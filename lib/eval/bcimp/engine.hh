/**
 * @defgroup bcimp Byte Code Interpreter with OpenMP support.
 * @ingroup eval
 *
 * This module collects all classes to compile and evaluate GiNaC expressions using a OpenMP enabled
 * byte-code interpreter. This interpreter evaluates independent expressions in separate threads,
 * which increases the execution speed for a large set of expressions, i.e. evaluation of jacobians.
 */


#ifndef __INA_EVALUATE_BCIMP_ENGINE_HH__
#define __INA_EVALUATE_BCIMP_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"


namespace iNA {
namespace Eval {
namespace bcimp {

/**
 * This class just collects all classes to comile and execute GiNaC expressions.
 *
 * @ingroup bcimp
 */
template <class InType, class OutType=InType>
class Engine
{
public:
  /** The code class for this engine. */
  typedef bcimp::Code Code;
  /** The compiler class of this engine. */
  typedef bcimp::Compiler<InType, OutType> Compiler;
  /** The interpreter class for this engine. */
  typedef bcimp::Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // __INA_EVALUATE_BCIMP_ENGINE_HH__
