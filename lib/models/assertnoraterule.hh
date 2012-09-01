#ifndef __FLUC_MODELS_ASSERTNORATERULE_HH__
#define __FLUC_MODELS_ASSERTNORATERULE_HH__

#include "basemodel.hh"


namespace iNA {
namespace Models {


/**
 * Simple assertion to test if the model as any rate-rule defined.
 *
 * @ingroup models
 */
class AssertNoRateRule
{
public:
  /**
   * Constructor, performs the test.
   *
   * @throws SBMLFeatureNotSupported If the model has a rate-rule.
   */
  AssertNoRateRule(BaseModel &model);
};


}
}

#endif // __FLUC_MODELS_ASSERTNORATERULE_HH__
