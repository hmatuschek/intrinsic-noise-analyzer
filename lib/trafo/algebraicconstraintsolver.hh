#ifndef __INA_TRAFO_ALGEBRAICCONSTRAINTSOLVER_HH__
#define __INA_TRAOF_ALGEBRAICCONSTRAINTSOLVER_HH__


#include "ast/ast.hh"


namespace iNA {
namespace Trafo {


/** This ultra cool class tries to transform all algebraic constraints on species defined in a
 * model into @c Ast::AssignmentRule for these species using GiNaCs @c lsolve function. The solution
 * is found symbolically under the assumption that the system of constraints forms a system of
 * linear equations in the species variables.
 *
 * If the solver is applied on a model, 3 cases can occur:
 *  - If the system of constraints is overdefined -> there is no solution and a @c RuntimeError
 *    exception is thrown.
 *  - If the system is exactly solvable (full rank), a @c RuntimeError exception is thrown as well
 *    as the solution suggests that the constraint species are constant.
 *  - However, if the system is underdefined i.e. there are less linare independent constraints then
 *    constraint species this operation tries to convert these constraints into
 *    @c Ast::AssignmentRule rules for the constraint species.
 *
 * As this procecudure is non-unique, the species without initial values are preferred to get an
 * assignment rule attached to minimize the chance to conflict with its initial value.
 *
 * Further, before applying this transformations, all @c Ast::AssingmentRule must be resolved.
 *
 * @ingroup trafo
 */
class AlgebraicConstraintSolver
{
protected:
  /** Will hold the list of @c Ast::AlgebraicConstraint instances being handled. */
  std::list<Ast::AlgebraicConstraint *> _algebraic_constraints;

  /** Will hold the list of algebraic constraints in the form 0 == f(s_1, s_2, ...); */
  GiNaC::lst _constraints;

  /** Will hold the list of constraint species. */
  GiNaC::lst _constraint_species;

protected:
  /** Hiddem constructor, assembles the list of constraints and constraint species.
   * Use static @c apply to resolve constraints. */
  AlgebraicConstraintSolver(Ast::Model &model);

  /** Returns true if the species appears in one of the constraints. */
  bool isSpeciesConstraint(Ast::Species *species);

  /** Returns true if the given expression is a valid solution (an expression with at least one
   * reference to a constraint species. */
  bool isValidSolution(GiNaC::ex expr);

public:
  /** This method performs the solving and substitution of algebraic constraints into assignment
   * rules. */
  static void apply(Ast::Model &model);
};

}
}

#endif
