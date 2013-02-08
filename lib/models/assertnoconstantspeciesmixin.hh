#ifndef __INA_MODELS_ASSERTNOCONSTANTSPECIESMIXIN_HH__
#define __INA_MODELS_ASSERTNOCONSTANTSPECIESMIXIN_HH__


#include "basemodel.hh"


namespace iNA {
namespace Models {


/**
 * Asserts, that there is no @c Ast::Species is defined as constant.
 *
 * @ingroup models
 */
class AssertNoConstantSpeciesMixin
{
public:
  /**
   * Checks if there is any species defined as constant.
   */
  AssertNoConstantSpeciesMixin(BaseModel &model);
};


}
}

#endif // __INA_MODELS_ASSERTNOCONSTANTSPECIESMIXIN_HH__
