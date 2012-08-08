#include "partialmodel.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Trafo;


PartialModel::PartialModel(Ast::Model &model)
  : numInd(0), numDep(0),
    permutationVector(model.numSpecies()), permutationM(),
    linkMatrixSource(), linkMatrix()
{
  // index of the first independent species
  size_t idx_ind = 0;
  // index of the first dependent species
  size_t idx_dep = model.numSpecies()-1;

  // Iterate over all species and assemble permutationVector
  for (size_t i=0;i<model.numSpecies(); i++) {
    // if species has a rule attached -> it is dependent
    if (model.getSpecies(i)->hasRule()) {
      permutationVector[i] = idx_dep; idx_dep--;
    } else {
      permutationVector[i] = idx_ind; idx_ind++;
    }
  }
  // determine number of independent and dependent species
  numInd = idx_ind; numDep = model.numSpecies() - numInd;
  // Create permutation matrix from vector:
  permutationM = Eigen::PermutationWrapper<Eigen::VectorXi>(permutationVector);

  // allocate some space for the link matrix:
  linkMatrixSource.resize(model.numSpecies(), numInd);
  linkMatrixSource.setZero(model.numSpecies(), numInd);
  linkMatrix.resize(model.numSpecies(), numInd);
  linkMatrix.setZero(model.numSpecies(), numInd);

  // now, assemble link-matrix:
  for (size_t i=0; i<model.numSpecies(); i++) {
    if (model.getSpecies(i)->hasRule()) {
      linkMatrixSource.row(i).noalias() = _createLinkVector(model, model.getSpecies(i)->getRule());
    } else {
      linkMatrixSource.coeffRef(i, permutationVector(i)) = 1;
    }
  }
  // permute rows into partitioning [indSpecis, depSpecies]:
  linkMatrixSource = permutationM * linkMatrixSource;
}


Eigen::VectorXex
PartialModel::_createLinkVector(Ast::Model &model, const Ast::Rule *rule)
{
  // Initialize link vector
  Eigen::VectorXex link(numInd); link.setZero(numInd);

  // Check type of rule:
  if (! Ast::Node::isAssignmentRule(rule)) {
    throw InternalError("Can not process rule: Rule is not an assignment rule!");
  }

  // get Rule expression
  GiNaC::ex rule_poly = rule->getRule();
  rule_poly = rule_poly.normal();

  // Assemble vector
  for (size_t i=0; i<model.numSpecies(); i++) {
    // get species symbol
    GiNaC::symbol sym = model.getSpecies(i)->getSymbol();

    // Check degree of rule in species sym:
    if (1 < rule_poly.degree(sym)) {
      InternalError err;
      err << "Assignment rule " << rule_poly << " is non-linear in species " << sym;
      throw err;
    }

    // get linear coefficient:
    GiNaC::ex coeff = rule_poly.coeff(sym, 1);
    if (permutationVector(i)>=int(numInd) && ! coeff.is_zero()) {
      InternalError err;
      err << "Assignment rule " << rule_poly << " depends on dependent species " << sym
          << ": Try to apply Trafo::AssignmentRuleInliner.";
      throw err;
    }

    // skip zeros (& dependent species):
    if (coeff.is_zero()) { continue; }

    // Store coefficient
    link(permutationVector(i)) = coeff;
  }

  return link;
}


void
PartialModel::evaluateLinkMatrix(Substitution &subs)
{
  for (int i=0; i<linkMatrixSource.rows(); i++) {
    for (int j=0; j<linkMatrixSource.cols(); j++) {
      // Get coefficient and apply substitutions:
      GiNaC::ex coeff = linkMatrixSource.coeff(i,j).subs(subs.getTable()).evalf();

      // Check if coefficient is a numeric value:
      if (! GiNaC::is_a<GiNaC::numeric>(coeff)) {
        InternalError err;
        err << "Can not evaluate link matrix, coefficient "
            << linkMatrixSource.coeff(i,j) << " at (" << unsigned(i) << ", " << unsigned(j) << ") "
            << "does not evaluate to a numeric value. Minimal expression " << coeff;
        throw err;
      }

      // store coefficient:
      linkMatrix.coeffRef(i,j) = GiNaC::ex_to<GiNaC::numeric>(coeff).to_double();
    }
  }
}
