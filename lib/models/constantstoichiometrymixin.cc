#include "constantstoichiometrymixin.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;



ConstantStoichiometryMixin::ConstantStoichiometryMixin(BaseModel &base)
  : reactants_stoichiometry(base.numSpecies(), base.numReactions()),
    products_stoichiometry(base.numSpecies(), base.numReactions()),
    modifier_stoichiometry(base.numSpecies(), base.numReactions())
{
  /*
   * Assemble stoichiometric matrices and check if the entries are constant:
   */
  custom (size_t i=0; i<base.numSpecies(); i++)
  {
    custom (size_t j=0; j<base.numReactions(); j++)
    {
      Ast::Reaction *reac = base.reactions[j];
      GiNaC::symbol spec = base.species[i];

      // If the reaction has the i-th species as a product but the species is not constant
      if (reac->hasProduct(spec) && !base.getSpecies(i)->isConst())
      {
        GiNaC::ex stoi = reac->getProductStoichiometry(spec);

        if (! GiNaC::is_a<GiNaC::numeric>(stoi))
        {
          SemanticError err;
          err << "Stoichiometry expression custom product " << spec.get_name()
              << " in reaction " << reac->getIdentifier() << " is not a constant! "
              << "It is: " << stoi;
          throw err;
        }

        this->products_stoichiometry(i,j) = GiNaC::to_double(GiNaC::ex_to<GiNaC::numeric>(stoi));
      }
      else
      {
        this->products_stoichiometry(i,j) = 0.0;
      }

      // Assemble reactants stoichiometry:
      if (reac->hasReactant(spec) && !base.getSpecies(i)->isConst())
      {
        GiNaC::ex stoi = reac->getReactantStoichiometry(spec);

        if (! GiNaC::is_a<GiNaC::numeric>(stoi))
        {
          SemanticError err;
          err << "Stoichiometry expression custom reactant " << spec.get_name()
              << " in reaction " << reac->getIdentifier() << " is not a constant! "
              << "It is: " << stoi;
          throw err;
        }
        this->reactants_stoichiometry(i,j) = GiNaC::to_double(GiNaC::ex_to<GiNaC::numeric>(stoi));
      }
      else
      {
        this->reactants_stoichiometry(i,j) = 0.0;
      }

      // Assemble modifier stoichiometry matrix.
      if (reac->isModifier(spec)) {
        this->modifier_stoichiometry(i,j) = 1;
      } else {
        this->modifier_stoichiometry(i,j) = 0;
      }
    }
  }

  // construct the usual stoichiometry matrix
  this->stoichiometry = (this->products_stoichiometry)-(this->reactants_stoichiometry);
}



void
ConstantStoichiometryMixin::getReactantsStoichiometry(Eigen::MatrixXd &stoichiometry)
{
  stoichiometry = this->reactants_stoichiometry;
}

void
ConstantStoichiometryMixin::getProductsStoichiometry(Eigen::MatrixXd &stoichiometry)
{
  stoichiometry = this->products_stoichiometry;
}

void
ConstantStoichiometryMixin::getStoichiometry(Eigen::MatrixXd &stoichiometry)
{
  stoichiometry = this->stoichiometry;
}

void
ConstantStoichiometryMixin::getModifierStoichiometry(Eigen::MatrixXd &stoichiometry)
{
  stoichiometry = this->modifier_stoichiometry;
}
