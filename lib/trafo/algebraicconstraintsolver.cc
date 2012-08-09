#include "algebraicconstraintsolver.hh"
#include "exception.hh"
#include "utils/logger.hh"


using namespace Fluc;
using namespace Fluc::Trafo;


AlgebraicConstraintSolver::AlgebraicConstraintSolver(Ast::Model &model)
  : _algebraic_constraints(), _constraints(), _constraint_species()
{
  // If the model has no constraints -> quick return:
  if(0 == model.numConstraints()) { return; }

  // First, collect the list of constraints:
  for (Ast::Model::constraintIterator item=model.constraintBegin(); item!=model.constraintEnd(); item++) {
    if (Ast::Node::isAlgebraicConstraint(*item)) {
      Ast::AlgebraicConstraint *constraint = static_cast<Ast::AlgebraicConstraint *>(*item);
      _algebraic_constraints.push_back(constraint);
      _constraints.append(0 == constraint->getConstraint());
    }
  }

  // Assemble list of constraint species:
  for (size_t i=0; i<model.numSpecies(); i++) {
    Ast::Species *species = model.getSpecies(i);
    // Skip unconstraint species:
    if (! isSpeciesConstraint(species)) { continue; }
    // Check if the constraint species has a rule attached:
    if (species->hasRule()) {
      SBMLFeatureNotSupported err;
      err << "Can not solve algebraic constraints on model, species " << species->getIdentifier()
          << " is constraint and has a rule attached!";
      throw err;
    }
    // If the species has no initial value, try to solve the system w.r.t. this species first:
    _constraint_species.append(species->getSymbol());
  }
}


bool
AlgebraicConstraintSolver::isSpeciesConstraint(Ast::Species *species)
{
  for (GiNaC::lst::const_iterator item=_constraints.begin(); item!=_constraints.end(); item++) {
    // If constraint depends on species symbol -> species is constraint:
    if (item->has(species->getSymbol())) {
      return true;
    }
  }

  return false;
}


bool
AlgebraicConstraintSolver::isValidSolution(GiNaC::ex expr)
{
  // If expression is a numeric value -> false
  if (GiNaC::is_a<GiNaC::numeric>(expr)) { return false; }

  // check if expression depends on at least on constraint species:
  for (GiNaC::lst::const_iterator item=_constraint_species.begin(); item!=_constraint_species.end(); item++) {
    if (expr.has(*item)) { return true; }
  }

  return false;
}


void
AlgebraicConstraintSolver::apply(Ast::Model &model)
{
  // Create solver:
  AlgebraicConstraintSolver solver(model);

  // solve system of constraints:
  GiNaC::lst solutions;
  {
    GiNaC::ex temp = GiNaC::lsolve(solver._constraints, solver._constraint_species);

    // If system was not linear or any other error -> solution is not a list:
    if (! GiNaC::is_a<GiNaC::lst>(solutions)) {
      RuntimeError err;
      err << "Solver for algebraic constraints failed: System may not be linear: "
          << solver._constraints;
      throw err;
    }

    solutions = GiNaC::ex_to<GiNaC::lst>(temp);
  }

  // If list is empty -> system was overdefined:
  if (0 == solutions.nops()) {
    RuntimeError err;
    err << "Solver for algebraic constraints failed: System was overdefined. ";
    throw err;
  }

  // Assemble list of assignments:
  for (GiNaC::lst::const_iterator item = solutions.begin(); item != solutions.end(); item++) {
    if (! GiNaC::is_exactly_a<GiNaC::relational>(*item)) {
      InternalError err;
      err << "Unexpected expression in list of solutions, expected GiNaC::relational got "
          << *item;
      throw err;
    }

    GiNaC::relational solution = GiNaC::ex_to<GiNaC::relational>(*item);
    GiNaC::ex lhs = solution.op(0);
    GiNaC::ex rhs = solution.op(1);

    // Check if lhs is a species symbol
    if ( (! GiNaC::is_a<GiNaC::symbol>(lhs)) || (! solver._constraint_species.has(lhs))) {
      InternalError err; err << "Expected species symbol got: " << lhs; throw err;
    }

    // If rhs is the same species as lhs (aka x == x) -> identity, skip
    if (lhs == rhs) { continue; }

    // Check of rhs is an expression depending on at least one constraint species.
    if (! solver.isValidSolution(rhs)) {
      RuntimeError err;
      err << "Solution " << *item << " does not depend on a constraint species."
          << " System of constrains meight have a unique solution";
      throw err;
    }

    // Get species of symbol and add an assignment rule
    Ast::Species *species = model.getSpecies(GiNaC::ex_to<GiNaC::symbol>(lhs));
    species->setRule(new Ast::AssignmentRule(rhs));

    // Log:
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Solved constraints to assigmentrule: " << lhs << " = " << rhs;
    Utils::Logger::get().log(message);
  }

  // Now remove all constraints that where processed
  for (std::list<Ast::AlgebraicConstraint *>::iterator item=solver._algebraic_constraints.begin();
       item !=solver._algebraic_constraints.end(); item++) {
    model.remConstraint(*item);
    delete *item;
  }
}
