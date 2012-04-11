#ifndef __FLUC_MODELS_ASSERTNOALGEBRAICCONSTRAINTMIXIN_HH__
#define __FLUC_MODELS_ASSERTNOALGEBRAICCONSTRAINTMIXIN_HH__

#include "basemodel.hh"


namespace Fluc {
namespace Models {


/**
 * Checks if the given model has any @c Ast::AlgebraicConstraint defined, if so, throws a
 * @c SBMLFeatureNotSupported exception.
 *
 * @ingroup models
 */
class AssertNoAlgebraicConstraintMixin
{
public:
  /**
   * Checks if there is a algebraic rule defined for the model.
   */
  AssertNoAlgebraicConstraintMixin(BaseModel &model);
};


}
}

#endif // __FLUC_MODELS_ASSERTNOALGEBRAICCONSTRAINTMIXIN_HH__
