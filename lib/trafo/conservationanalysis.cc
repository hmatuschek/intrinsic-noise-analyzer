#include "conservationanalysis.hh"
#include "exception.hh"
#include "ginacsupportforeigen.hh"
#include "utils/logger.hh"


using namespace Fluc;
using namespace Fluc::Trafo;


ConservationAnalysis::ConservationAnalysis(Ast::Model &model)
  : _conserved()
{
  // Assemble vector of initially independent species:
  Eigen::VectorXex species(model.numSpecies());
  size_t numSpecies=0;
  for (size_t i=0; i<model.numSpecies(); i++) {
    if (! model.getSpecies(i)->hasRule()) {
      species(numSpecies) = model.getSpecies(i)->getSymbol(); numSpecies++;
    }
  }

  // Assemble reduced stoichiometry:
  Eigen::MatrixXd stoichiometry(numSpecies, model.numReactions());

  for (size_t j=0; j<model.numReactions(); j++) {
    Ast::Reaction *reaction = model.getReaction(j);
    for (size_t i=0; i<numSpecies; i++) {
      if (reaction->hasProduct(GiNaC::ex_to<GiNaC::symbol>(species(i)))) {
        GiNaC::ex coeff = reaction->getProductStoichiometry(GiNaC::ex_to<GiNaC::symbol>(species(i)));
        if (! GiNaC::is_a<GiNaC::numeric>(coeff.evalf())) {
          SBMLFeatureNotSupported err;
          err << "Can not perform conservation analysis: Stoichiometry of product "
              << species(i) << " is not a numeric constant.";
          throw err;
        }
        stoichiometry(i,j) = GiNaC::ex_to<GiNaC::numeric>(coeff).to_double();
      } else {
        stoichiometry(i,j) = 0;
      }

      if (reaction->hasReactant(GiNaC::ex_to<GiNaC::symbol>(species(i)))) {
        GiNaC::ex coeff = reaction->getReactantStoichiometry(GiNaC::ex_to<GiNaC::symbol>(species(i)));
        if (! GiNaC::is_a<GiNaC::numeric>(coeff.evalf())) {
          SBMLFeatureNotSupported err;
          err << "Can not perform conservation analysis: Stoichiometry of reactant "
              << species(i) << " is not a numeric constant.";
          throw err;
        }
        stoichiometry(i,j) = -GiNaC::ex_to<GiNaC::numeric>(coeff).to_double();
      }
    }
  }

  // Perfrom conservation analysis:
  Eigen::FullPivLU< Eigen::MatrixXd > LU;
  LU.compute(stoichiometry.transpose());

  // Assemble list of dependent and independent species as well as the link matrix:
  size_t n_dep  = LU.dimensionOfKernel();
  size_t n_indep = numSpecies - n_dep;
  Eigen::PermutationMatrix<Eigen::Dynamic> pM(LU.permutationQ().transpose());
  Eigen::VectorXex ind_spec = (pM * species.head(numSpecies)).head(n_indep);
  Eigen::VectorXex dep_spec = (pM * species.head(numSpecies)).tail(n_dep);
  Eigen::MatrixXd linkMatrix = -(pM*LU.kernel()).transpose();


  // Assemble expressions for assignment rules
  // for some reason, the scalar product of double-matrix and expression vector does not work:
  for (size_t i=0; i<n_dep; i++) {
    GiNaC::ex link = 0;

    // product \vec{link} = linkMatrix * \vec{ind_species}:
    for (size_t j=0; j<n_indep; j++) {
      link += linkMatrix(i,j)*ind_spec(j);
    }

    // Store assignment list
    _conserved.append(dep_spec(i) == link);
    // Add assignment rule to dependent species:
    Ast::Species *species_obj = model.getSpecies(GiNaC::ex_to<GiNaC::symbol>(dep_spec(i)));
    species_obj->setRule(new Ast::AssignmentRule(link));

    // Nice log message...
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Found conserved cycle: " << dep_spec(i) << " = " << link;
    Utils::Logger::get().log(message);
  }
}


void
ConservationAnalysis::apply(Ast::Model &model)
{
  ConservationAnalysis analysis(model);
}
