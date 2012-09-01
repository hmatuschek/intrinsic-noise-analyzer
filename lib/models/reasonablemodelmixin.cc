#include "reasonablemodelmixin.hh"

using namespace iNA;
using namespace iNA::Models;


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
