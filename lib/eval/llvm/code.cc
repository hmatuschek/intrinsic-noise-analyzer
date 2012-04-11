#include "code.hh"
#include "exception.hh"
#include <iostream>


using namespace Fluc::Evaluate::LLVM;

Code::Code()
  : context(), module("system", context), builder(context), function(0), input(0), output(0),
    complex_t(0), real_pow(0), complex_pow(0), engine(0), function_ptr(0)
{
  // Init target and create module:
  llvm::InitializeNativeTarget();

  { // Assemble function interface:
    function = llvm::cast<llvm::Function>(
          module.getOrInsertFunction(
            "system", llvm::Type::getVoidTy(context), llvm::Type::getInt8PtrTy(context),
            llvm::Type::getInt8PtrTy(context), (llvm::Type *)0));
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(entry);

    // Get function arguments:
    llvm::Function::arg_iterator item = this->function->arg_begin();
    this->input = item; item++;
    this->output = item; item++;

    // Assign some names:
    this->input->setName("inptr");
    this->output->setName("outptr");
  }

  { // Define extern pow() function from libm:
    std::vector<const llvm::Type *> args(2, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *signature = llvm::FunctionType::get(
          llvm::Type::getDoubleTy(context), args, false);
    this->real_pow = llvm::Function::Create(
          signature, llvm::Function::DLLImportLinkage, "pow", &module);
  }
}


Code::~Code()
{
  // Remove module from engine:
  if (0 != this->engine) {
    this->engine->removeModule(&module);
    delete this->engine;
  }
}


llvm::IRBuilder<> &
Code::getBuilder()
{
  return builder;
}


llvm::LLVMContext &
Code::getContext()
{
  return context;
}


void
Code::compile(size_t opt_level)
{
  // Verify module:
  {
    std::string error_string;
    if(llvm::verifyModule(module, llvm::ReturnStatusAction, &error_string)) {
      InternalError err;
      err << "Can not verify LLVM IR module: " << error_string;
      throw err;
    }
  }

  // Create Execution engine:
  {
    std::string error_string;
    llvm::EngineBuilder builder(&module);
    builder.setErrorStr(&error_string);

    if( 0 == (engine = builder.create()) )
    {
      // Free module:
      InternalError err;
      err << "Can not create LLVM execution engine: " << error_string;
      throw err;
    }
  }

  /*// Perform some optimizations:
  llvm::FunctionPassManager fpm(&module);

  // Set up the optimizer pipeline.  Start with registering info about how the
  // target lays out data structures.
  fpm.add(new llvm::TargetData(*engine->getTargetData()));
  // Provide basic AliasAnalysis support for GVN.
  fpm.add(llvm::createBasicAliasAnalysisPass());

  if (0 < opt_level) {
    // Promote allocas to registers.
    fpm.add(llvm::createPromoteMemoryToRegisterPass());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm.add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    fpm.add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    fpm.add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    fpm.add(llvm::createCFGSimplificationPass());
  }

  fpm.doInitialization();

  fpm.run(*function);*/

  // Get function pointer:
  this->function_ptr = this->engine->getPointerToFunction(function);
}


void *
Code::getFunctionPtr()
{
  return this->function_ptr;
}


void
Code::exec(const std::vector<llvm::GenericValue> &args)
{
  this->engine->runFunction(this->function, args);
}


llvm::Value *
Code::getInput()
{
  return this->input;
}

llvm::Value *
Code::getOutput()
{
  return this->output;
}


llvm::Type *
Code::getComplexTy()
{
  return this->complex_t;
}


llvm::Function *
Code::getRealPowFunction()
{
  return this->real_pow;
}


llvm::Function *
Code::getComplexPowFunction()
{
  return this->complex_pow;
}
