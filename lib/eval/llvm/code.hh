#ifndef __FLUC_EVALUATE_LLVM_CODE_HH__
#define __FLUC_EVALUATE_LLVM_CODE_HH__

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/Instructions.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
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
  llvm::LLVMContext context;
  llvm::Module module;
  llvm::IRBuilder<> builder;
  llvm::Function *function;

  llvm::Value *input;
  llvm::Value *output;

  llvm::Type *complex_t;
  llvm::Function *real_pow;
  llvm::Function *complex_pow;

  llvm::ExecutionEngine *engine;
  void *function_ptr;


public:
  Code();

  ~Code();

  llvm::IRBuilder<> &getBuilder();

  llvm::LLVMContext &getContext();

  void *getFunctionPtr();

  llvm::Value *getInput();
  llvm::Value *getOutput();

  llvm::Type     *getComplexTy();
  llvm::Function *getRealPowFunction();
  llvm::Function *getComplexPowFunction();

  void compile(size_t opt_level=0);

  void exec(const std::vector<llvm::GenericValue> &args);
};


}
}
}


#endif // CODE_HH
