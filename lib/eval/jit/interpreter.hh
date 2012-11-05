#ifndef __FLUC_EVALUATE_LLVM_INTERPRETER_HH__
#define __FLUC_EVALUATE_LLVM_INTERPRETER_HH__

#include "code.hh"
#include "exception.hh"



namespace iNA {
namespace Eval {
namespace jit {


/**
 * "Interpreter" custom the compiled LLVM IR. This class executes the compiled LLVM IR
 * stored in the associated @c Code instance.
 *
 * @ingroup jit
 */
template <class InType, class OutType=InType>
class Interpreter
{
protected:
  /**
   * Holds the code instance to execute.
   */
  Code *code;

  /**
   * Holds a weak reference to the compiled function implementing the system.
   */
  void (*system_function)(const double *, double *);


public:
  /**
   * Default constructor. Call @c setCode to assign a code-instance.
   */
  Interpreter()
    : code(0), system_function(0)
  {
    // Pass...
  }

  /**
   * Constructor with code-object.
   */
  Interpreter(Code *code)
    : code(0), system_function(0)
  {
    setCode(code);
  }

  /**
   * (Re-) Sets the code to be executed.
   */
  void setCode(Code *code) {
    this->code = code;
    this->system_function = 0;
  }

  /**
   * Runs the compiled system. First, checks if the system was allready compiled. If not,
   * the function is requested from the @c Code object. Then executes the function directly.
   */
  inline void run(const typename InType::Scalar *input, typename OutType::Scalar *output)
  {
    // Ensure, that we have a pointer to the compiled function:
    if (0 == this->system_function)
      this->system_function = (void (*)(const double *, double *)) this->code->getFunctionPtr();
    // Call it:
    this->system_function(input, output);
  }

  /**
   * Executes the code using Eigen vectors or matrices.
   */
  inline void run(const InType &input, OutType &output)
  {
    // Unpack pointers
    const typename InType::Scalar *inptr = (const typename InType::Scalar *)input.data();
    typename OutType::Scalar *outptr = (typename OutType::Scalar *)output.data();
    // customward call:
    this->run(inptr, outptr);
  }
};


}
}
}

#endif // INTERPRETER_HH
