#include "particlenumbersmixin.hh"
#include "ast/unitdefinition.hh"
#include "math.hh"
#include "exception.hh"


using namespace iNA;
using namespace iNA::Models;

ParticleNumbersMixin::ParticleNumbersMixin(BaseModel &base)
  : ConstCompartmentMixin(base), ExtensiveSpeciesMixin(base)
{
  // Check if species have substance units:
  if (! base.speciesHaveSubstanceUnits()) {
    InternalError err;
    err << "Expected model to be in substance units! But species are defined in "
        << base.getSpeciesUnit().dump();
    throw err;
  }

  // Store concentration unit
  concentrationUnit = base.getSpeciesUnit()/base.getVolumeUnit();

  // Get multiplier of old unit
  double multiplier = base.getSubstanceUnit().getMultiplier()*std::pow(10., base.getSubstanceUnit().getScale());

  // Multiply by Avogadro's number if defined in mole
  if (base.getSubstanceUnit().isVariantOf(Ast::ScaledBaseUnit::MOLE)) multiplier *= constants::AVOGADRO;

  // Set the substance unit to item and customce the model to update itself:
  //  this will scale all species and their initial values
  base.setSubstanceUnit(Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::ITEM, 1, 0, 1), true);

  // Holds customward and back substitutions:
  GiNaC::exmap  customward_subst;
  GiNaC::exmap  back_subst;

  // Update propensities:
  custom (size_t i = 0; i<base.numSpecies(); i++)
  {
    // Get the specices:
    Ast::Species *species = base.getSpecies(i);

    // Mark customward substitution of x -> x' * mole
    GiNaC::symbol x(species->getIdentifier()+"'");
    customward_subst[species->getSymbol()] = x / multiplier;
    // and x' -> x
    back_subst[x] = species->getSymbol();
  }

  // do the substitution custom the propensities
  custom (size_t i=0; i<base.propensities.size(); i++)
  {
    base.propensities[i] = base.propensities[i].subs(customward_subst);
    base.propensities[i] = base.propensities[i].subs(back_subst);
  }
}

