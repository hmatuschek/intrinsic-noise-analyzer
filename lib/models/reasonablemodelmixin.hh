#ifndef __INA_MODELS_REASONABLEMODELMIXIN_HH__
#define __INA_MODELS_REASONABLEMODELMIXIN_HH__

#include "basemodel.hh"
#include "assertnoconstantspeciesmixin.hh"
#include "assertconstantparametermixin.hh"
#include "assertnoassignmentrule.hh"
#include "assertnoraterule.hh"
#include "assertirreversiblereactions.hh"
#include "trafo/assertions.hh"


namespace iNA {
namespace Models {


/**
 * This Mixin unifies some assumptions we make, that are needed to be satisfied by a
 * "reasonable" or "well-defined" model.
 *
 * This mixin is a convenience class unifying a collection of certain mixins and assertions needed
 * to be satisfied custom a "well-defined" model. The assertions include:
 *
 * @li Test if model is not explicit time-dependent (by @c AssertNoExplicitTimeDependenceMixin)
 * @li Test if there is no @c Ast::AlgebraicConstraint defined.
 *     (by @c AssertNoAlgebraicConstraintMixin)
 * @li Test if there is no @c Ast::Species defined as constant. (by @c AssertNoConstantSpeciesMixin)
 * @li Test if there is no variable @c Ast::Parameter defined. (by @c AssertConstantParametersMixin)
 * @li Test if there is no assignment-rule defined. (by @c AssertNoAssignmentRule)
 * @li Test if there is no rate-rule defined. (by @c AssertNoRateRule)
 *
 * @ingroup models
 */
class ReasonableModelMixin :
    public AssertConstantParameterMixin,
    public AssertNoAssignmentRule,
    public AssertNoRateRule,
    public AssertIrreversibleReactions

{
public:
  /**
   * Checks all constraints to be satisfied by a "reasonable" model.
   */
  ReasonableModelMixin(BaseModel &model);
};


}
}

#endif
