/**
 * @defgroup direct Direct GiNaC Expression Evaluation.
 * @ingroup eval
 *
 * This module collects all classes needed to evaluate GiNaC expressions directly. This kind of
 * evaluation is just used for testing the more efficient methods like @ref bci and @ref jit.
 */


#ifndef __INA_EVALUATE_DIRECT_ENGINE_HH__
#define __INA_EVALUATE_DIRECT_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"

namespace iNA {
namespace Eval {
namespace direct {


/** This template just collects the classes needed to perform the direct evaluation of GiNaC
 * expressions. This kind of evaluation is very slow, as GiNaC uses multiprecision arithmetic
 * to perform the evaluation. Therefore this engine is just used to test the more efficient
 * engines like @ref bci or @ref jit.
 *
 * @ingroup direct
 */
template<class InType, class OutType=InType>
class Engine
{
public:
  /** Defines the "code" class. */
  typedef direct::Code Code;
  /** Defines the "compiler" class. */
  typedef direct::Compiler<InType, OutType> Compiler;
  /** Defines the interpreter class. */
  typedef direct::Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // ENGINE_HH
