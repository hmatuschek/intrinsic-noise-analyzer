#include "intensivespeciesmixin.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;

IntensiveSpeciesMixin::IntensiveSpeciesMixin(BaseModel &base)
{
  // Holds forward and back substitutions:
  GiNaC::exmap  forward_subst;
  GiNaC::exmap  back_subst;

  for (size_t i = 0; i<base.numSpecies(); i++)
  {
    // Get the specices:
    Ast::Species *species = base.getSpecies(i);

    // Obtains the unit for the i-th species:
    const Ast::Unit &unit = species->getUnit();

    // Check if variable is an intensive concentration unit:
    if (unit.isConcentrationUnit())
    {
      // All fine, continue.
      continue;
    }

    // If it is not an extensive substance unit -> try to convert it:
    if (unit.isSubstanceUnit())
    {
      // Get compartment
      Ast::Compartment *compartment = species->getCompartment();

      // Fix it: first, divide unit by volume unit of species compartment:
      species->setUnit(species->getUnit()/compartment->getUnit());

      // Finally fix initial value (concentration) if set:
      if (species->hasValue())
      {
        species->setValue(species->getValue() / compartment->getValue());
      }

      // Mark forward substitution of x -> x' * Volume
      GiNaC::symbol x(species->getIdentifier()+"'");
      forward_subst[species->getSymbol()] = x * compartment->getSymbol();

      // and x' -> x
      back_subst[x] = species->getSymbol();
    }
    else
    {
      SemanticError err;
      std::stringstream str; species->getUnit().dump(str);
      err << "Can not convert species " << species->getName()
          << " with unit " << str.str()
          << " into intensive units";
      throw err;
    }
  }

  Trafo::Substitution forward_pass(forward_subst); base.apply(forward_pass);
  Trafo::Substitution back_pass(back_subst); base.apply(back_pass);

  for (size_t i=0; i<base.propensities.size(); i++)
  {
      base.propensities[i] = base.propensities[i].subs(forward_subst);
      base.propensities[i] = base.propensities[i].subs(back_subst);
  }
}
