#ifndef __INA_EVALUATE_LLVM_ENGINE_HH__
#define __INA_EVALUATE_LLVM_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"

namespace Fluc {
namespace Evaluate {
namespace LLVM {


template<class InType, class OutType=InType>
class Engine
{
public:
  typedef Code Code;
  typedef Compiler<InType, OutType> Compiler;
  typedef Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // ENGINE_HH
