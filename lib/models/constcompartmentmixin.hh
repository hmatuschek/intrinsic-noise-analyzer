#ifndef __FLUC_MODULE_CONSTCOMPARTMENTMIXIN_HH__
#define __FLUC_MODULE_CONSTCOMPARTMENTMIXIN_HH__

#include "basemodel.hh"
#include "baseunitmixin.hh"
#include "ginacsupportforeigen.hh"

namespace Fluc {
namespace Models {


/**
 * Asserts, that there is for each species a compartment of constant size > 0.
 *
 * @ingroup models
 */
class ConstCompartmentMixin
        : public BaseUnitMixin
{
protected:
  /**
   * Holds the vector of constant compartment volumes for each species.
   */
  Eigen::VectorXex volumes;

public:
  /**
   * Performs the checks on the given base-model.
   */
  ConstCompartmentMixin(BaseModel &base);
};


}
}

#endif // CONSTCOMPARTMENTMIXIN_HH
