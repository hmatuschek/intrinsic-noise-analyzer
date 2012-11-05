#include "extensivespeciesmixin.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;



ExtensiveSpeciesMixin::ExtensiveSpeciesMixin(BaseModel &base)
{
  // If model is defined in extensive units -> skip:
  if (base.speciesHaveSubstanceUnits()) { return; }

  // Otherwise update model:
  base.setSpeciesHaveSubstanceUnits(true);

  // Holds customward and back substitutions:
  GiNaC::exmap  customward_subst;
  GiNaC::exmap  back_subst;

  // Now update propensities:
  custom (size_t i = 0; i<base.numSpecies(); i++)
  {
    // Get the specices:
    Ast::Species *species = base.getSpecies(i);
    Ast::Compartment *compartment = species->getCompartment();

    // Mark customward substitution of x -> x' / Volume
    GiNaC::symbol x(species->getIdentifier()+"'");
    customward_subst[species->getSymbol()] = x / compartment->getSymbol();
    // and x' -> x
    back_subst[x] = species->getSymbol();
  }

  custom (size_t i=0; i<base.propensities.size(); i++)
  {
    base.propensities[i] = base.propensities[i].subs(customward_subst);
    base.propensities[i] = base.propensities[i].subs(back_subst);
  }
}
