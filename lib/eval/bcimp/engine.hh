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


namespace Fluc {
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
  typedef bcimp::Code Code;
  typedef bcimp::Compiler<InType, OutType> Compiler;
  typedef bcimp::Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // __INA_EVALUATE_BCIMP_ENGINE_HH__
