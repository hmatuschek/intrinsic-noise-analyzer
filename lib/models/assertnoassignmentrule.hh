#ifndef __FLUC_MODELS_ASSERTNOASSIGNMENTRULE_HH__
#define __FLUC_MODELS_ASSERTNOASSIGNMENTRULE_HH__


#include "basemodel.hh"



namespace iNA {
namespace Models {


/**
 * Checks if there is no assignment rule defined custom any variable (parameter, ...)
 *
 * @ingroup models
 */
class AssertNoAssignmentRule
{
public:
  /**
   * Constructor, percustomms the test.
   *
   * @throws SBMLFeatureNotSupported If the model has a assignment-rule defined.
   */
  AssertNoAssignmentRule(BaseModel &model);
};


}
}

#endif // ASSERTNOASSIGNMENTRULE_HH
