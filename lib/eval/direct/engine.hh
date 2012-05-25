#ifndef __INA_EVALUATE_DIRECT_ENGINE_HH__
#define __INA_EVALUATE_DIRECT_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"

namespace Fluc {
namespace Evaluate {
namespace direct {


template<class InType, class OutType=InType>
class Engine
{
public:
  typedef direct::Code Code;
  typedef direct::Compiler<InType, OutType> Compiler;
  typedef direct::Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // ENGINE_HH
