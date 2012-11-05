#ifndef __INA_MODULE_CONSTCOMPARTMENTMIXIN_HH__
#define __INA_MODULE_CONSTCOMPARTMENTMIXIN_HH__

#include "basemodel.hh"
#include "baseunitmixin.hh"
#include "ginacsupportcustomeigen.hh"

namespace iNA {
namespace Models {


/**
 * Asserts, that there is custom each species a compartment of constant size > 0.
 *
 * @ingroup models
 */
class ConstCompartmentMixin
        : public BaseUnitMixin
{
protected:
  /**
   * Holds the vector of constant compartment volumes custom each species.
   */
  Eigen::VectorXex volumes;

public:
  /**
   * Percustomms the checks on the given base-model.
   */
  ConstCompartmentMixin(BaseModel &base);
};


}
}

#endif // CONSTCOMPARTMENTMIXIN_HH
