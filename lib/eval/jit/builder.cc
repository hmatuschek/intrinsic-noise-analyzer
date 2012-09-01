#include "builder.hh"

using namespace iNA::Eval::jit;


llvm::Value *
Builder<double>::createIndex(Code *code, size_t index)
{
  return code->getBuilder().getInt64(index);
}


llvm::Value *
Builder<double>::createConstant(Code *code, const GiNaC::numeric &value)
{
  return llvm::ConstantFP::get(code->getBuilder().getDoubleTy(), value.to_double());
}


void
Builder<double>::createStore(Code *code, llvm::Value *value, size_t index)
{
  // First, get function argument and cast it to double *
  llvm::Value *arg = code->getBuilder().CreatePointerCast(
        code->getOutput(), code->getBuilder().getDoubleTy()->getPointerTo(), "outvec");
  llvm::Value *elm_idx = createIndex(code, index);
  llvm::Value *elm_ptr = code->getBuilder().CreateGEP(arg, elm_idx, "elm");
  code->getBuilder().CreateStore(value, elm_ptr, false);
}


llvm::Value *
Builder<double>::createLoad(Code *code, size_t index)
{
  // First, get function argument and cast it to double *
  llvm::Value *arg = code->getBuilder().CreatePointerCast(
        code->getInput(), code->getBuilder().getDoubleTy()->getPointerTo(), "invec");
  llvm::Value *elm_idx = createIndex(code, index);
  llvm::Value *elm_ptr = code->getBuilder().CreateGEP(arg, elm_idx, "elm");
  return code->getBuilder().CreateLoad(elm_ptr, false, "value");
}


llvm::Value *
Builder<double>::createAdd(Code *code, llvm::Value *lhs, llvm::Value *rhs)
{
  // Simply create "+" operation:
  return code->getBuilder().CreateFAdd(lhs, rhs, "add");
}


llvm::Value *
Builder<double>::createMul(Code *code, llvm::Value *lhs, llvm::Value *rhs)
{
  // Simply create "*" operation:
  return code->getBuilder().CreateFMul(lhs, rhs, "mul");
}


llvm::Value *
Builder<double>::createPow(Code *code, llvm::Value *lhs, llvm::Value *rhs)
{
  return code->getBuilder().CreateCall2(code->getRealPowFunction(), lhs, rhs);
}

llvm::Value *
Builder<double>::createAbs(Code *code, llvm::Value *arg)
{
  return code->getBuilder().CreateCall(code->getRealAbsFunction(), arg);
}

llvm::Value *
Builder<double>::createLog(Code *code, llvm::Value *arg)
{
  return code->getBuilder().CreateCall(code->getRealLogFunction(), arg);
}

llvm::Value *
Builder<double>::createExp(Code *code, llvm::Value *arg)
{
  return code->getBuilder().CreateCall(code->getRealExpFunction(), arg);
}
