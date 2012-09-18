#include "reasonablemodelmixin.hh"

using namespace iNA;
using namespace iNA::Models;


ReasonableModelMixin::ReasonableModelMixin(BaseModel &model) :
  AssertNoAlgebraicConstraintMixin(model),
  AssertConstantParameterMixin(model),
  AssertNoAssignmentRule(model),
  AssertNoRateRule(model),
  AssertIrreversibleReactions(model)
{
  // Check if there is no explicit time-dependence:
  Trafo::NoExplicitTimeDependenceAssertion::apply(model);

  // Pass...
}
