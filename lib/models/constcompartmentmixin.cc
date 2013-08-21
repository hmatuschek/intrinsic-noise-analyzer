#include "constcompartmentmixin.hh"
#include "math.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


ConstCompartmentMixin::ConstCompartmentMixin(BaseModel &base)
  : BaseUnitMixin(base), volumes(base.numSpecies())
{
  // First, check if all compartments are constant:
  for (size_t i=0; i<base.numCompartments(); i++)
  {
    Ast::Compartment *comp = base.getCompartment(i);

    if (! comp->isConst())
    {
      SBMLFeatureNotSupported err;
      err << "This implementation can not handle non-constant compartments yet.";
      throw err;
    }
  }

  // define the compartment volumes for all species
  GiNaC::numeric fac=1.;
  if(base.getSubstanceUnit().hasVariantOf(Ast::Unit::MOLE)) {
    double multiplier = base.getSubstanceUnit().getMultiplier();
    multiplier *= std::pow(10., base.getSubstanceUnit().getScale());
    fac = multiplier*constants::AVOGADRO;
  }

  // Get compartment volumes for all species in SBML model:
  for (size_t i=0; i<base.numSpecies(); i++)
  {

    this->volumes(i) = fac * base.getSpecies(i)->getCompartment()->getSymbol();

    if ( 0 >= this->volumes[i] )
    {
      SemanticError err;
      err << "Encountered a compartment with volume <= 0."
          << base.getSpecies(i)->getCompartment()->getIdentifier() << ".";
      throw err;
    }
  }
}


