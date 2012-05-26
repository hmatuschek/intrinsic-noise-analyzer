/**
 * @defgroup jit
 * @ingroup eval
 *
 * This module collects all classes used to JIT compile and evaluate expressions using the LLVM
 * JIT backend.
 */

#ifndef __INA_EVALUATE_LLVM_ENGINE_HH__
#define __INA_EVALUATE_LLVM_ENGINE_HH__

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"

namespace Fluc {
namespace Evaluate {
/**
 * This namespace collect all classes for the JIT compiler execution engine used for model
 * evaluations.
 *
 * @todo Rename it to Fluc::Evaluate::JTI, as LLVM is the only JIT backend used here, libjit was
 *       dropped.
 *
 * @ingroup jit
 */
namespace LLVM {


/**
 * This class just defines the code, compiler and "interpreter" classes for the JIT engine. This
 * allows to implement simple generic models and simulators as C++ templates.
 *
 * @ingroup jit
 */
template<class InType, class OutType=InType>
class Engine
{
public:
  /**
   * Defines the type of the code object, holding the compiled code to be executed.
   */
  typedef LLVM::Code Code;

  /**
   * An instance of this class can be used to compile some expressions into code.
   */
  typedef LLVM::Compiler<InType, OutType> Compiler;

  /**
   * An instance of this class finally executes the compiled code.
   */
  typedef LLVM::Interpreter<InType, OutType> Interpreter;
};


}
}
}

#endif // ENGINE_HH
