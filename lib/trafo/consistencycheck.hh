#ifndef __INA_TRAFO_CONSISTENCYCHECK_HH__
#define __INA_TRAFO_CONSISTENCYCHECK_HH__

#include <ast/model.hh>
#include <exception.hh>

namespace iNA {
namespace Trafo {

/** This class implements a simple consistency check that ensures there are not cyclic dependencies
 * in the initial value definitions.
 * @ingroup trafo */
class ConsistencyCheck
{
public:
  /** This method checks if there are some cyclic dependencies in the initial value definitions
   * of any variables of the given model. A @c SemanticError exception will be thrown if there
   * are any cyclic dependencies. */
  static void assertConsistent(Ast::Model *model) throw (SemanticError);

  /** Identical to @c assertConsistent, but returns false instead of throwing an exception if there
   * are any cyclic dependencies in the initial values of the model. */
  static bool check(Ast::Model *model);
};

}
}

#endif // __INA_TRAFO_CONSISTENCYCHECK_HH__
