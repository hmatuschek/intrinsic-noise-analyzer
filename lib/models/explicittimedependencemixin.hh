#ifndef __FLUC_MODELS_EXPLICITTIMEDEPENDENCEMIXIN_HH__
#define __FLUC_MODELS_EXPLICITTIMEDEPENDENCEMIXIN_HH__

#include "basemodel.hh"

namespace Fluc {
namespace Models {


/**
 * Helper class to test a @c BaseModel if any expression is explicit time-dependent.
 *
 * @ingroup models
 */
class ExplicitTimeDependenceWalker : public Ast::Trafo::Walker
{
protected:
  /**
   * Holds a weak reference to the model being processed.
   */
  BaseModel &model;


public:
  /**
   * Constructs the walker.
   */
  ExplicitTimeDependenceWalker(BaseModel &model)
    : model(model)
  {
    // Pass..
  }

  /**
   * Handle all expressions.
   */
  virtual void handleExpression(GiNaC::ex node)
  {
    // Check if expression explicitly depends on model-global time symbol.
    if (model.isExplTimeDep(node))
    {
      SBMLFeatureNotSupported err;
      err << "Explicit time-dependent expressions are not supported yet.";
      throw err;
    }
  }
};



/**
 * Tests if any expression in the given base-model (@c Models::BaseModel) depends explicitly on
 * the time.
 *
 * @ingroup models
 */
class AssertNoExplicitTimeDependenceMixin
{
public:
  /**
   * Constructs a test for explicit time-dependence.
   */
  AssertNoExplicitTimeDependenceMixin(BaseModel &model);
};


}
}

#endif // EXPLICITTIMEDEPENDENCEMIXIN_HH
