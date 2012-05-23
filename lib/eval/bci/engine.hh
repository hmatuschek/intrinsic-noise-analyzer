#ifndef __INA_EVALUATE_BCI_ENGINE_HH__
#define __INA_EVALUATE_BCI_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"


namespace Fluc {
namespace Evaluate {
namespace bci {

/**
 * Just defines a namespace holding the code, compiler and interpreter types associated with the
 * byte-code interpreter engine.
 */
template<class InType, class OutType = InType>
class Engine {
public:
  /** The code class, an instance of this class holds the byte-code to be executed. */
  typedef Code Code;
  typedef Compiler<InType, OutType> Compiler;
  typedef Interpreter<InType, OutType> Interpreter;
};

}
}
}
#endif // ENGINE_HH
