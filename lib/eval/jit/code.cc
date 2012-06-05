#include "code.hh"
#include "exception.hh"
#include <iostream>
#include <llvm/DerivedTypes.h>
#include <llvm/PassManager.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>


using namespace Fluc::Eval::jit;


Code::Code(size_t num_threads)
  : context(llvm::getGlobalContext()), module(0), builder(context),
    function(0), input(0), output(0), complex_t(0),
    real_pow(0), complex_pow(0), real_abs(0), real_log(0), real_exp(0),
    engine(0), function_ptr(0)
{
  // Init target and create module:
  llvm::InitializeNativeTarget();
  module = new llvm::Module("system", context);

  { // Assemble function interface:
    this->function = llvm::cast<llvm::Function>(
          module->getOrInsertFunction(
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

  { // Define complex<double> ABI type
    this->complex_t = llvm::StructType::get(
          context, llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context),
          (const llvm::Type *)0);
  }

  { // Define extern pow() function from libm:
    std::vector<const llvm::Type *> args(2, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *signature = llvm::FunctionType::get(
          llvm::Type::getDoubleTy(context), args, false);
    this->real_pow = llvm::Function::Create(
          signature, llvm::Function::DLLImportLinkage, "pow", module);
  }

  { // Define extern abs() function from libm:
    std::vector<const llvm::Type *> args(1, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *signature = llvm::FunctionType::get(
          llvm::Type::getDoubleTy(context), args, false);
    this->real_abs = llvm::Function::Create(
          signature, llvm::Function::DLLImportLinkage, "abs", module);
  }

  { // Define extern log() function from libm:
    std::vector<const llvm::Type *> args(1, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *signature = llvm::FunctionType::get(
          llvm::Type::getDoubleTy(context), args, false);
    this->real_log = llvm::Function::Create(
          signature, llvm::Function::DLLImportLinkage, "log", module);
  }

  { // Define extern exp() function from libm:
    std::vector<const llvm::Type *> args(1, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *signature = llvm::FunctionType::get(
          llvm::Type::getDoubleTy(context), args, false);
    this->real_exp = llvm::Function::Create(
          signature, llvm::Function::DLLImportLinkage, "exp", module);
  }
}



Code::~Code()
{
  if (0 != engine) {
    // Remove module from engine:
    this->engine->removeModule(module);
    delete this->engine;
  }

  delete module;
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


llvm::Function *
Code::getSystem()
{
  return this->function;
}

void *
Code::getFunctionPtr()
{
  return this->function_ptr;
}

void
Code::setFunctionPtr(void *ptr)
{
  this->function_ptr = ptr;
}

void
Code::setEngine(llvm::ExecutionEngine *engine)
{
  this->engine = engine;
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


llvm::Function *
Code::getRealAbsFunction()
{
  return this->real_abs;
}


llvm::Function *
Code::getRealLogFunction()
{
  return this->real_log;
}


llvm::Function *
Code::getRealExpFunction()
{
  return this->real_exp;
}


llvm::Module *
Code::getModule()
{
    return module;
}