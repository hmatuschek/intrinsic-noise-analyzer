#include "constcompartmentmixin.hh"
#include "math.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Models;


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
  if(this->getSubstanceUnit().hasVariantOf(Ast::ScaledBaseUnit::MOLE))
    fac = this->getMultiplier()*constants::AVOGADRO;

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


