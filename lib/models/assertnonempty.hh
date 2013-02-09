#ifndef __INA_MODELS_ASSERTNONEMPTY_HH__
#define __INA_MODELS_ASSERTNONEMPTY_HH__

#include "basemodel.hh"


namespace iNA {
namespace Models {


/**
 * Simple assertion to test if the model is empty.
 *
 * @ingroup models
 */
class AssertNonEmpty
{
public:
  /**
   * Constructor, performs the test.
   *
   * @throws SBMLFeatureNotSupported If the model has a rate-rule.
   */
  AssertNonEmpty(BaseModel &model);
};


}
}

#endif // __INA_MODELS_ASSERTNORATERULE_HH__
