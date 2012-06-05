#include "reasonablemodelmixin.hh"

using namespace Fluc;
using namespace Fluc::Models;


ReasonableModelMixin::ReasonableModelMixin(BaseModel &model) :
  AssertNoExplicitTimeDependenceMixin(model),
  AssertNoAlgebraicConstraintMixin(model),
  AssertConstantParameterMixin(model),
  AssertNoAssignmentRule(model),
  AssertNoRateRule(model),
  AssertIrreversibleReactions(model)
{
  // Pass...
}
