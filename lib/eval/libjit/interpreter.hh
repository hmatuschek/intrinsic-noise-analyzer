#ifndef __FLUC_EVALUATE_LIBJIT_INTERPRETER_HH__
#define __FLUC_EVALUATE_LIBJIT_INTERPRETER_HH__

#include "code.hh"


namespace Fluc {
namespace Evaluate {

/**
 * This namespace holds all classes implementing an execution engine using libjit.
 *
 * @bug Note: The JIT compiler is still marked as exprimental since there is an unresolved bug
 *      in the handleing of complex values.
 *
 * @ingroup eval
 */
namespace libjit {


/**
 * Executes precompiled libjit IR in its JIT compiler.
 *
 * @ingroup eval
 */
template <class InType, class OutType=InType>
class Interpreter
{
protected:
  /**
   * Holds the compiled code.
   */
  Code *code;

public:
  /**
   * Constructor.
   */
  Interpreter()
    : code(0)
  {
    // Pass...
  }


  /**
   * Constructor.
   *
   * Also sets the code to interprete.
   */
  Interpreter(Code *code)
    : code(code)
  {
    // Pass...
  }


  /**
   * (Re-) Sets the code to interprete.
   */
  void setCode(Code *code)
  {
    this->code = code;
  }


  /**
   * Performs the evaluation.
   */
  void run(const InType &input, OutType &output)
  {
    // Assemble function arguments:
    typename InType::Scalar *in_ptr  = (typename InType::Scalar *)(input.data());
    typename OutType::Scalar *out_ptr = (typename OutType::Scalar *)(output.data());
    void *args[2] = {&in_ptr, &out_ptr};

    // Call JIT compiled function:
    jit_function_apply(this->code->getFunction(), args, 0);

    // Thats all Folks!
  }
};


}
}
}
#endif
