#include "assertnoalgebraicconstraintmixin.hh"

using namespace Fluc;
using namespace Fluc::Models;


AssertNoAlgebraicConstraintMixin::AssertNoAlgebraicConstraintMixin(BaseModel &model)
{
  if (0 != model.getNumConstraints())
  {
    SBMLFeatureNotSupported err;
    err << "This implementation can not yet handle algebraic rules applied to the model.";
    throw err;
  }
}
