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
  if (! base.speciesHasSubstanceUnits()) {
    InternalError err;
    err << "Expected model to be in substance units! But species are defined in "
        << base.getSpeciesUnit().dump();
    throw err;
  }

  // If model is defined in items -> skip:
  if (base.getSubstanceUnit().isVariantOf(Ast::ScaledBaseUnit::ITEM)) { return; }

  // Otherwise force species units to be items:
  double multiplier = constants::AVOGADRO * base.getSubstanceUnit().getMultiplier();
  multiplier *= std::pow(10., base.getSubstanceUnit().getScale());
  base.setSubstanceUnit(Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::ITEM, 1, 0, 1));

  // Holds forward and back substitutions:
  GiNaC::exmap  forward_subst;
  GiNaC::exmap  back_subst;

  // Update propensities:
  for (size_t i = 0; i<base.numSpecies(); i++)
  {
    // Get the specices:
    Ast::Species *species = base.getSpecies(i);

    // Mark forward substitution of x -> x' * mole
    GiNaC::symbol x(species->getIdentifier()+"'");
    forward_subst[species->getSymbol()] = x / multiplier;
    // and x' -> x
    back_subst[x] = species->getSymbol();
  }

  // do the substitution for the propensities
  for (size_t i=0; i<base.propensities.size(); i++)
  {
    base.propensities[i] = base.propensities[i].subs(forward_subst);
    base.propensities[i] = base.propensities[i].subs(back_subst);
  }
}

