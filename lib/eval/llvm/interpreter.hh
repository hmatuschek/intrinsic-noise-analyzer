#ifndef __FLUC_EVALUATE_LLVM_INTERPRETER_HH__
#define __FLUC_EVALUATE_LLVM_INTERPRETER_HH__

#include "code.hh"
#include "exception.hh"



namespace Fluc {
namespace Evaluate {
namespace LLVM {


template <class InType, class OutType=InType>
class Interpreter
{
protected:
  Code *code;

public:
  Interpreter(Code *code)
    : code(code)
  {
    // Pass...
  }


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
