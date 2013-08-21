#ifndef __INA_EVALUATE_LLVM_CODE_HH__
#define __INA_EVALUATE_LLVM_CODE_HH__

#include "config.hh"
#if INA_LLVM_VERSION_IS_33
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#else
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#endif

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>
#if INA_LLVM_VERSION_IS_2X
#include <llvm/Target/TargetSelect.h>
#elif INA_LLVM_VERSION_IS_3X
#include <llvm/Support/TargetSelect.h>
#endif
#if INA_LLVM_VERSION_IS_32
#include <llvm/IRBuilder.h>
#elif INA_LLVM_VERSION_IS_33
#include <llvm/IR/IRBuilder.h>
#else
#include <llvm/Support/IRBuilder.h>
#endif


namespace iNA {
namespace Eval {
namespace jit {

/** Holds the LLVM IR implementing the expressions.
 * @ingroup jit */
class Code
{
public:
  /** Constructor, allocates some empty code. */
  Code(size_t num_treads=1);
  /** Destructor. */
  ~Code();

  /** Returns a reference to the LLVM IR builder. */
  llvm::IRBuilder<> &getBuilder();
  /** Returns the current LLVM context, usually the global context. */
  llvm::LLVMContext &getContext();

  /** Retunrs the pointer to the function once the code was compliled. */
  void *getFunctionPtr();
  /** (Re-) Sets the pointer to the compiled function. */
  void setFunctionPtr(void *ptr);
  /** Sets the execution engine. */
  void setEngine(llvm::ExecutionEngine *engine);

  /** Returns the LLVM IR function. */
  llvm::Function *getSystem();
  /** Returns the LLVM IR module (holding the function). */
  llvm::Module *getModule();

  /** Returns the LLVM IR input value. */
  llvm::Value *getInput();
  /** Returns the LLVM IR output value. */
  llvm::Value *getOutput();

  /** The complex data type (defined in the module). */
  llvm::Type     *getComplexTy();
  /** A reference to the real valued pow() function (INA RUNTIME). */
  llvm::Function *getRealPowFunction();
  /** A reference to the complex valued pow() function (INA RUNTIME). */
  llvm::Function *getComplexPowFunction();
  llvm::Function *getRealAbsFunction();
  llvm::Function *getRealLogFunction();
  llvm::Function *getRealExpFunction();

  void exec(const std::vector<llvm::GenericValue> &args);

protected:
  /** Holds a weak reference to the global LLVM context. */
  llvm::LLVMContext &context;
  /** A pointer to the LLVM module, holding the IR code. */
  llvm::Module *module;
  /** A assistent to assemble LLVM IR. */
  llvm::IRBuilder<> builder;
  /** A pointer to the function being assembled. */
  llvm::Function *function;

  /** Holds a pointer to the input vector. */
  llvm::Value *input;
  /** Holds a pointer to the output vector. */
  llvm::Value *output;

  /** The complex type. */
  llvm::Type *complex_t;
  /** Holds a reference to libm's pow() function. */
  llvm::Function *real_pow;
  /** Holds a reference to libm's cpow() function. */
  llvm::Function *complex_pow;
  /** Holds a reference to libm's fabs() function. */
  llvm::Function *real_abs;
  /** Holds a reference to libm's log() function. */
  llvm::Function *real_log;
  /** Holds a reference to  libm's exp() function. */
  llvm::Function *real_exp;

  /** This will point to the execution engine, the instance will be created by the @c Compiler. */
  llvm::ExecutionEngine *engine;

  /** Once all code is assembles as LLVM IR, this pointer will hold the address of the compiled
   * function. */
  void *function_ptr;
};


}
}
}


#endif // CODE_HH
