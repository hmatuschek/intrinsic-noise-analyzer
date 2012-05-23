#ifndef __FLUC_EVAL_HH__
#define __FLUC_EVAL_HH__


/**
 * @defgroup eval Expression Evaluation
 *
 * This group contains a set of classes implementing different evaluation mechanisms from very slow
 * direct evaluation of GiNaC expressions to JIT compilers.
 *
 * All evaluation methods share the same interface, so they are selectable by build-flags. The
 * direct evaluation method using GiNaCs internal methods pass @c -DEXECUTION_ENGINE=DIRECT
 * to cmake. This will define the @c Fluc::Evaluate::direct namespace as the Fluc::Eval namespace.
 * To use the portable byte-code interpreter pass @c -DEXECUTION_ENGINE=BYTECODE to cmake. This
 * will define the @c Fluc::Evaluate::bci namespace as the Fluc::Eval namespace. This engine
 * will be used by default. To use the JIT compiler of libjit, pass @c -DEXECUTION_ENGINE=LIBJIT
 * to cmake. This will define the @c Fluc::Evaluate::libjit namespace as the Fluc::Eval namespace.
 * Also the libjit is added as a dependency. This method is the fastest, as it compiles the
 * expressions as machine-code before evaluation.
 *
 * @code
 * // Allocate the code object:
 * Fluc::Eval::Code code;
 *
 * // Construct a compiler:
 * Fluc::Eval::Compiler<Eigen::VectorXd> compiler(&code, index_table);
 *
 * // Compile some expressions ...
 *
 * // ... and finalize compilation.
 * compiler.finalize();
 *
 * // Now, construct an interpreter ...
 * Fluc::Eval::Interpreter<Eigen::VectorXd> interpreter(&code);
 *
 * // ... and evaluate ...
 * interpreter.run(in_vector, out_vector);
 * @endcode
 *
 * What ever engine is selected as the default execution-engine, you can always access all
 * engines directly by their own namespace, in this example for the byte-code interpreter:
 *
 * @code
 * Fluc::Evaluate::bci::Code code;
 * Fluc::Evaluate::bci::Compiler<Eigen::VectorXd> compiler(&code, index_table);
 * Fluc::Evaluate::bci::Interpreter<Eigen::VectorXd> interpreter(&code);
 * @endcode
 *
 */

#if EXECUTION_ENGINE == 1
#include "direct/code.hh"
#include "direct/compiler.hh"
#include "direct/interpreter.hh"

#warning "Slow GiNaC evaluation is used as default!"

namespace Fluc {
namespace Eval = Fluc::Evaluate::direct;
}

#elif EXECUTION_ENGINE == 3
#include "bcimp/code.hh"
#include "bcimp/compiler.hh"
#include "bcimp/interpreter.hh"
namespace Fluc {
namespace Eval = Fluc::Evaluate::bcimp;
}

#elif EXECUTION_ENGINE == 4
#include "libjit/code.hh"
#include "libjit/compiler.hh"
#include "libjit/interpreter.hh"

#warning "Untested libjit evaluation is used as default!"

namespace Fluc {
namespace Eval = Fluc::Evaluate::libjit;
}

#elif EXECUTION_ENGINE == 5
//#include "llvm/code.hh"
//#include "llvm/compiler.hh"
//#include "llvm/interpreter.hh"

#warning "Untested LLVM evaluation is used as default!"

namespace Fluc {
namespace Eval = Fluc::Evaluate::LLVM;
}

#else // Use ByteCodeInterpreter (BCI) by default (EXECUTION_ENGINE==2)
#include "bci/code.hh"
#include "bci/compiler.hh"
#include "bci/interpreter.hh"

namespace Fluc {
namespace Eval = Fluc::Evaluate::bci;
}
#endif


#endif  
