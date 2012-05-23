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

public:
  /**
   * Default constructor. Call @c setCode to assign a code-instance.
   */
  Interpreter()
    : code(0)
  {
    // Pass...
  }

  /**
   * Constructor with code-object.
   */
  Interpreter(Code *code)
    : code(0)
  {
    setCode(code);
  }

  /**
   * (Re-) Sets the code to be executed.
   */
  void setCode(Code *code) {
    this->code = code;
  }

  /**
   * Executes the code.
   */
  inline void run(const InType &input, OutType &output)
  {
    const typename InType::Scalar *inptr = (const typename InType::Scalar *)input.data();
    typename OutType::Scalar *outptr = (typename OutType::Scalar *)output.data();

    std::vector<llvm::GenericValue> args(2);
    args[0].PointerVal = (void *)inptr; args[1].PointerVal = outptr;

    this->code->exec(args);
  }
};


}
}
}

#endif // INTERPRETER_HH
