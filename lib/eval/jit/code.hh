#ifndef __FLUC_EVALUATE_LLVM_CODE_HH__
#define __FLUC_EVALUATE_LLVM_CODE_HH__

#include "config.hh"
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>
#if INA_LLVM_VERSION_IS_2X
#include <llvm/Target/TargetSelect.h>
#elif INA_LLVM_VERSION_IS_3X
#include <llvm/Support/TargetSelect.h>
#endif
#include <llvm/Support/IRBuilder.h>


namespace Fluc {
namespace Eval {
namespace jit {

/**
 * Holds the LLVM IR implementing the expressions.
 *
 * @ingroup jit
 */
class Code
{
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
   * funciton. */
  void *function_ptr;


public:
  Code(size_t num_treads=1);

  ~Code();

  llvm::IRBuilder<> &getBuilder();

  llvm::LLVMContext &getContext();

  void *getFunctionPtr();
  void setFunctionPtr(void *ptr);

  void setEngine(llvm::ExecutionEngine *engine);

  llvm::Function *getSystem();
  llvm::Module *getModule();

  llvm::Value *getInput();
  llvm::Value *getOutput();

  llvm::Type     *getComplexTy();
  llvm::Function *getRealPowFunction();
  llvm::Function *getComplexPowFunction();
  llvm::Function *getRealAbsFunction();
  llvm::Function *getRealLogFunction();
  llvm::Function *getRealExpFunction();

  void exec(const std::vector<llvm::GenericValue> &args);
};


}
}
}


#endif // CODE_HH
