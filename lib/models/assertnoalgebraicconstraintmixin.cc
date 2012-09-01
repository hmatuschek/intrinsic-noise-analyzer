#include "assertnoalgebraicconstraintmixin.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


AssertNoAlgebraicConstraintMixin::AssertNoAlgebraicConstraintMixin(BaseModel &model)
{
  if (0 != model.numConstraints())
  {
    SBMLFeatureNotSupported err;
    err << "This implementation can not yet handle algebraic rules applied to the model.";
    throw err;
  }
}
