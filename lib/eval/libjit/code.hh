#ifndef __FLUC_EVALUATE_LIBJIT_CODE_HH__
#define __FLUC_EVALUATE_LIBJIT_CODE_HH__

#include <jit/jit.h>

namespace Fluc {
namespace Evaluate {
namespace libjit {


/**
 * This class implements the "code" object holding the pre-compiled expression to be evaluated by
 * libjit.
 *
 * @ingroup eval
 */
class Code
{
protected:
  /**
   * Holds the JIT context.
   */
  jit_context_t context;

  /**
   * The function, actually implementing the code.
   */
  jit_function_t function;


public:
  /**
   * Creates an empty code-object.
   */
  Code();

  /**
   * Destructor, also frees the JIT context.
   */
  ~Code();

  /**
   * Returns the function-object, to be compiled.
   */
  jit_function_t getFunction();
};


}
}
}

#endif
