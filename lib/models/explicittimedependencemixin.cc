#include "explicittimedependencemixin.hh"

using namespace Fluc;
using namespace Fluc::Models;



AssertNoExplicitTimeDependenceMixin::AssertNoExplicitTimeDependenceMixin(BaseModel &model)
{
  //
  ExplicitTimeDependenceWalker walker(model);
  walker.handleModule(&model);

  // and now, check if propensities are expl. time-dependent:
  for (size_t i=0; i<model.propensities.size(); i++)
  {
    if (model.isExplTimeDep(model.propensities[i]))
    {
      SBMLFeatureNotSupported err;
      err << "Explicit time-dependent propensities are not supported yet.";
      throw err;
    }
  }
}
