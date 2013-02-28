#ifndef __INA_MODELS_ASSERTCONSTANTPARAMETERMIXIN_HH__
#define __INA_MODELS_ASSERTCONSTANTPARAMETERMIXIN_HH__


#include "basemodel.hh"


namespace iNA {
namespace Models {


/**
 * Checks if all defined parameters (including reaction-local parameters) are constant and have an
 * initial value assigned.
 *
 * @ingroup models
 */
class AssertConstantParameterMixin
{
public:
  /**
   * Checks the assertion.
   *
   * @throws SBMLFeatureNotSupported If a parameter is defined to be variable.
   */
  AssertConstantParameterMixin(BaseModel &model);


protected:
  /**
   * Helper function to test if all parameter definitions are constant.
   */
  void checkParameter(Ast::Parameter *parameter);
};


}
}

#endif // __INA_MODELS_ASSERTCONSTANTPARAMETERMIXIN_HH__
