#ifndef __FLUC_MODELS_PRELNAMIXIN_HH__
#define __FLUC_MODELS_PRELNAMIXIN_HH__

#include "basemodel.hh"
#include "constcompartmentmixin.hh"
#include "intensivespeciesmixin.hh"
#include "reasonablemodelmixin.hh"

namespace Fluc {
namespace Models {

/**
 * This mixin derives the reaction rates to leading order in the volume, see Ref. \cite vanKampen.
 *
 * The microscopic rate function of a reaction is given by
 * \f[ \hat{f}_i(\vec{n}) = \frac{a_i(\vec{n})}{\Omega}, \f]
 * where \f$ a_i \f$ is the propensity of the \f$i\f$-th reaction, \f$\vec{n}\f$ are the reactants and
 * \f$ \Omega \f$ is the volume of a specific compartment.
 * Then the microscopic rate function can be expanded as
 * \f[ \hat{f}_i(\vec{n}/\Omega) = f_i([\vec{X}]) + \frac{1}{\Omega} f_i^{(1)}([\vec{X}]) + ... \f]
 * where \f$ [\vec{X}] \f$ is the vector of macroscopic concentrations.
 *
 * The mixin performs the algebraic substitution \f$\vec{n}/\Omega \to [\vec{X}]\f$
 * and the subsequent Taylor expansion.
 *
 * @ingroup models
 */

class preLNAMixin :
    public IntensiveSpeciesMixin,
    public ConstCompartmentMixin,
    public ReasonableModelMixin
{
protected:
  /**
   * holds intensive vector of reaction rates  \f$ \vec{f}(\vec{n}/\Omega) \f$.
   */
  GiNaC::exvector rates;

  /**
   * holds first correction to vector of reaction rates  \f$ \vec{f}^{(1)}(\vec{n}/\Omega) \f$.
   */
  GiNaC::exvector rates1;

public:
  /**
   * Constructor...
   */
  preLNAMixin(BaseModel &base);
};


}
}

#endif
