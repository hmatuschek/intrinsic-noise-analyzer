#include "particlenumbersmixin.hh"
#include "ast/unitdefinition.hh"
#include "math.hh"
#include "exception.hh"


using namespace iNA;
using namespace iNA::Models;

ParticleNumbersMixin::ParticleNumbersMixin(BaseModel &base)
    : ConstCompartmentMixin(base),ExtensiveSpeciesMixin(base)
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

      // Check if variable is an extensive substance unit:
      if (unit.isConcentrationUnit())
      {
          throw InternalError("Something went wrong here. Expected amount of substrate but got concentration.");
      }

      // If it is not an extensive substance unit -> try to convert it:
      if (unit.isSubstanceUnit())
      {

        if(unit.isVariantOf(Ast::ScaledBaseUnit::MOLE,1.0))
        {

            GiNaC::numeric fac = GiNaC::numeric(constants::AVOGADRO);

            // convert to particle numbers
            if (species->hasValue())
            {

                fac = fac*this->getMultiplier();
                species->setValue(fac*species->getValue());

            // Mark forward substitution of x -> x' * mole
            GiNaC::symbol x(species->getIdentifier()+"'");
            forward_subst[species->getSymbol()] = x / fac;
            // and x' -> x
            back_subst[x] = species->getSymbol();

            // set unit
            /***
            * @ todo: set correct unit ITEM.
            */

            //variable->setUnit(unit.asScaledUnit().getBaseUnit());

            }
        }
        else if(unit.isVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS,1.0))
        {
            // pass
        }
        else if(unit.isVariantOf(Ast::ScaledBaseUnit::ITEM,1.0))
        {
            // pass
        }
        else
        {
            SemanticError err;
            std::stringstream str; species->getUnit().dump(str);
            err << "Can not convert amount of " << species->getName()
                << " with unit " << str.str()
                << " into particle numbers";
            throw err;
        }

    }

    }


    Trafo::Substitution forward_pass(forward_subst); base.apply(forward_pass);
    Trafo::Substitution back_pass(back_subst); base.apply(back_pass);

    // do the substitution for the propensities
    for (size_t i=0; i<base.propensities.size(); i++)
    {
        base.propensities[i] = base.propensities[i].subs(forward_subst);
        base.propensities[i] = base.propensities[i].subs(back_subst);
    }
}

