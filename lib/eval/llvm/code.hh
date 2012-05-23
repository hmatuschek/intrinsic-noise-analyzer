#ifndef __FLUC_EVALUATE_LLVM_CODE_HH__
#define __FLUC_EVALUATE_LLVM_CODE_HH__

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Support/IRBuilder.h>


namespace Fluc {
namespace Evaluate {

/**
 * This namespace holds the classes of the execution engine using the LLVM compiler infrastructure.
 *
 * @ingroup eval
 */
namespace LLVM {

/**
 * Holds the LLVM IR implementing the expressions.
 *
 * @ingroup eval
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

  llvm::Value *input;
  llvm::Value *output;

  llvm::Type *complex_t;
  llvm::Function *real_pow;
  llvm::Function *complex_pow;
  llvm::Function *real_abs;
  llvm::Function *real_log;
  llvm::Function *real_exp;

  llvm::ExecutionEngine *engine;
  void *function_ptr;


public:
  Code();

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
