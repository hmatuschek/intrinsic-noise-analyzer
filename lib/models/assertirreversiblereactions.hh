#ifndef __INA_MODELS_ASSERTIRREVERSIBLEREACTIONS_HH__
#define __INA_MODELS_ASSERTIRREVERSIBLEREACTIONS_HH__

#include "basemodel.hh"

namespace iNA {
namespace Models {


/**
 * Checks if the model is not defined as reversible.
 *
 * @ingroup models
 */
class AssertIrreversibleReactions
{
public:
  /**
   * Constructor, test if the model is irreversible.
   *
   * @throws SBMLFeatureNotSupported If the model is defined to be reversible.
   */
  AssertIrreversibleReactions(BaseModel &model);
};


}
}

#endif // ASSERTIRREVERSIBLEREACTIONS_HH
