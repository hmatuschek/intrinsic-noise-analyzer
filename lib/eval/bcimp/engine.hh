#ifndef __INA_EVALUATE_BCIMP_ENGINE_HH__
#define __INA_EVALUATE_BCIMP_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"


namespace Fluc {
namespace Evaluate {
namespace bcimp {


template <class InType, class OutType=InType>
class Engine
{
public:
  typedef bcimp::Code Code;
  typedef bcimp::Compiler<InType, OutType> Compiler;
  typedef bcimp::Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // __INA_EVALUATE_BCIMP_ENGINE_HH__
