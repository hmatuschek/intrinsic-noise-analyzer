/**
 * @defgroup bci Byte Code Interpreter
 * @ingroup eval
 *
 * This group collects all classes implementing the byte code interpreter. This kind of execution
 * engine compiles GiNaC expressions in a series of instructions, that are executed in a virtual
 * stack machine, the interpreter. This allows to evaluate GiNaC expressions in an efficient way,
 * while conserving the platform independence of the direct GiNaC evaluation. The compiled byte
 * code can then be optimized to increase the execution speed.
 */

#ifndef __INA_EVALUATE_BCI_ENGINE_HH__
#define __INA_EVALUATE_BCI_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"


namespace iNA {
namespace Eval {
namespace bci {

/**
 * Just defines a namespace holding the code, compiler and interpreter types associated with the
 * byte-code interpreter engine.
 *
 * @ingroup bci
 */
template<class InType, class OutType = InType>
class Engine {
public:
  /** The code class, an instance of this class holds the byte-code to be executed. */
  typedef bci::Code Code;
  typedef bci::Compiler<InType, OutType> Compiler;
  typedef bci::Interpreter<InType, OutType> Interpreter;
};

}
}
}
#endif // ENGINE_HH
