#ifndef __FLUC_COMPILER_ASSIGNMENTRULEINLINER_HH__
#define __FLUC_COMPILER_ASSIGNMENTRULEINLINER_HH__

#include "pass.hh"


namespace iNA {
namespace Ast {
namespace Trafo {


/**
 * Replaces all references to variables defined with a @c Ast::AssignmentRule by the
 * assigned expression, that determines the value of the variable.
 *
 * @ingroup ast
 */
class AssignmentRuleInliner : public Pass
{
public:
  /**
   * Constructs a new AssignmentRuleInliner.
   */
  AssignmentRuleInliner();

  /**
   * Searches for variables haveing a @c AssignmentRule attached in the given module.
   */
  virtual void handleModule(Ast::Module *node);
};


}
}
}

#endif // ASSIGNMENTRULEINLINER_HH
