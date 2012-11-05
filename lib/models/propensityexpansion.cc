#include "propensityexpansion.hh"

using namespace iNA;
using namespace iNA::Models;

propensityExpansion::propensityExpansion(BaseModel &base):
  IntensiveSpeciesMixin(base), ConstCompartmentMixin(base), ReasonableModelMixin(base),
  rates(base.numReactions()),rates1(base.numReactions())
{

  // Iterate over all reactions
  custom (size_t i=0; i<base.numReactions(); i++)
  {

    this->rates[i]  = base.propensities[i];
    this->rates1[i] = 0;

    custom(size_t j=0; j<base.numCompartments(); j++)
    {

     if( base.propensities[i].has(base.getCompartment(j)->getSymbol()))
     {

        /*
        * ... if symbol is present attempt a taylor expansion
        */

        // define variable custom inverse volume
        GiNaC::symbol inverseVolume;
        GiNaC::ex volume = base.getCompartment(j)->getSymbol();

        // and use it to obtain taylor series
        // and principal part

        GiNaC::ex taylor;
        GiNaC::ex principalPart;

        try {

            // substitute inverse volume
            GiNaC::ex temp = (this->rates[i]).subs(volume==1/inverseVolume);

            // ...and do the expansion
            taylor = temp.series(inverseVolume==0,1);
            principalPart = series_to_poly(temp.series(inverseVolume==0,-1));

        } catch (std::runtime_error err) {

            // must set to zero if expansion fails
            taylor = 0;
            principalPart = 0;
        }

        if(!principalPart.is_zero() || taylor.coeff(inverseVolume,-1).is_zero())
        {
            // simply skip if principal part is non-zero
        }
        else
        {
            // use expansion of rates
            this->rates[i]  = volume*taylor.coeff(inverseVolume,-1);
            this->rates1[i] = taylor.coeff(inverseVolume,0);
        }
     }
    }

    // ...done

  }

}
