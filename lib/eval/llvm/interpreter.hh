#ifndef __FLUC_EVALUATE_LLVM_INTERPRETER_HH__
#define __FLUC_EVALUATE_LLVM_INTERPRETER_HH__

#include "code.hh"
#include "exception.hh"



namespace Fluc {
namespace Evaluate {
namespace LLVM {


/**
 * "Interpreter" for the compiled LLVM IR. This class executes the compiled LLVM IR
 * stored in the associated @c Code instance.
 */
template <class InType, class OutType=InType>
class Interpreter
{
protected:
  /**
   * Holds the code instance to execute.
   */
  Code *code;

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

  inline void run(const typename InType::Scalar *input, typename OutType::Scalar *output)
  {
    // Ensure, that we have a pointer to the compiled function:
    if (0 == this->system_function)
      this->system_function = (void (*)(const double *, double *)) this->code->getFunctionPtr();
    // Call it:
    this->system_function(input, output);
  }

  /**
   * Executes the code.
   */
  inline void run(const InType &input, OutType &output)
  {
    // Unpack pointers
    const typename InType::Scalar *inptr = (const typename InType::Scalar *)input.data();
    typename OutType::Scalar *outptr = (typename OutType::Scalar *)output.data();
    // Forward call:
    this->run(inptr, outptr);
  }
};


}
}
}

#endif // INTERPRETER_HH
