#include "compiler.hh"
#include <llvm/PassManager.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils/PromoteMemToReg.h>
#include <llvm/Analysis/InstructionSimplify.h>
#include <llvm/Analysis/Dominators.h>

#include "utils/logger.hh"
#include "utils/cputime.hh"


using namespace Fluc::Evaluate::LLVM;



CompilerCore::CompilerCore(Code *code)
  : code(code)
{
  // Pass...
}


void
CompilerCore::setCode(Code *code)
{
  this->code = code;
}


void
CompilerCore::finalize(size_t level)
{
  // create "return void" instruction:
  code->getBuilder().CreateRetVoid();

  // First, verify module:
  {
    std::string error_string;
    if(llvm::verifyModule(*(code->getModule()), llvm::ReturnStatusAction, &error_string)) {
      InternalError err;
      err << "Can not verify LLVM IR module: " << error_string;
      throw err;
    }
  }

  // Create Execution engine:
  llvm::ExecutionEngine *engine=0;
  {
    std::string error_string;
    llvm::EngineBuilder builder(code->getModule());
    builder.setErrorStr(&error_string);
    builder.setEngineKind(llvm::EngineKind::JIT);

    if (0 == (engine = builder.create())) {
      InternalError err;
      err << "Can not create LLVM-JIT execution engine: " << error_string;
      throw err;
    }
    code->setEngine(engine);
  }

  // Perform some optimizations:
  llvm::FunctionPassManager fpm(code->getModule());
  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  fpm.add(new llvm::TargetData(*engine->getTargetData()));
  // Provide basic AliasAnalysis support for GVN.
  fpm.add(llvm::createBasicAliasAnalysisPass());
  if (0 < level) {
    // Promote allocas to registers.
    fpm.add(llvm::createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm.add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    fpm.add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    fpm.add(llvm::createGVNPass());
    // Constant propagation
    fpm.add(llvm::createConstantPropagationPass());
    // Remove dead instructions
    fpm.add(llvm::createDeadInstEliminationPass());
  }
  fpm.doInitialization();

  Utils::Message msg = LOG_MESSAGE(Utils::Message::DEBUG);
  Utils::CpuTime clock; clock.start();
  fpm.run(*code->getSystem());
  msg << "Optimized LLVM IR code in " << clock.stop() << "s.";
  Utils::Logger::get().log(msg);

  //code->getModule()->dump();

  // Get function pointer:
  code->setFunctionPtr(engine->getPointerToFunction(code->getSystem()));
}

