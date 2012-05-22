#ifndef __FLUC_EVALUATE_LLVM_BUILDER_HH__
#define __FLUC_EVALUATE_LLVM_BUILDER_HH__

#include "code.hh"
#include <ginac.h>


namespace Fluc {
namespace Evaluate {
namespace LLVM {


/**
 * Template interface for all internal used builders.
 */
template <typename Scalar>
class Builder
{
public:
  static llvm::Value *createIndex(size_t index);
  static llvm::Value *createConstant(Code *code, const GiNaC::numeric &value);
  static void createStore(Code *code, llvm::Value *value, size_t index);
  static llvm::Value *createLoad(Code *code, size_t index);
  static llvm::Value *createAdd(Code *code, llvm::Value *lhs, llvm::Value *rhs);
  static llvm::Value *createMul(Code *code, llvm::Value *lhs, llvm::Value *rhs);
  static llvm::Value *createPow(Code *code, llvm::Value *lhs, llvm::Value *rhs);
};


/**
 * Specialization for real-valued expressions.
 */
template<>
class Builder<double>
{
public:
  static llvm::Value *createIndex(Code *code, size_t index)
  {
    return code->getBuilder().getInt64(index);
  }


  static llvm::Value *createConstant(Code *code, const GiNaC::numeric &value)
  {
    return llvm::ConstantFP::get(code->getBuilder().getDoubleTy(), value.to_double());
  }


  static void createStore(Code *code, llvm::Value *value, size_t index)
  {
    // First, get function argument and cast it to double *
    llvm::Value *arg = code->getBuilder().CreatePointerCast(
          code->getOutput(), code->getBuilder().getDoubleTy()->getPointerTo(), "outvec");
    llvm::Value *elm_idx = createIndex(code, index);
    llvm::Value *elm_ptr = code->getBuilder().CreateGEP(arg, elm_idx, "elm");
    code->getBuilder().CreateStore(value, elm_ptr, false);
  }


  static llvm::Value *createLoad(Code *code, size_t index)
  {
    // First, get function argument and cast it to double *
    llvm::Value *arg = code->getBuilder().CreatePointerCast(
          code->getInput(), code->getBuilder().getDoubleTy()->getPointerTo(), "invec");
    llvm::Value *elm_idx = createIndex(code, index);
    llvm::Value *elm_ptr = code->getBuilder().CreateGEP(arg, elm_idx, "elm");
    return code->getBuilder().CreateLoad(elm_ptr, false, "value");
  }


  static llvm::Value *createAdd(Code *code, llvm::Value *lhs, llvm::Value *rhs)
  {
    // Simply create "+" operation:
    return code->getBuilder().CreateFAdd(lhs, rhs, "add");
  }

  static llvm::Value *createMul(Code *code, llvm::Value *lhs, llvm::Value *rhs)
  {
    // Simply create "*" operation:
    return code->getBuilder().CreateFMul(lhs, rhs, "mul");
  }

  static llvm::Value *createPow(Code *code, llvm::Value *lhs, llvm::Value *rhs)
  {
    return code->getBuilder().CreateCall2(code->getRealPowFunction(), lhs, rhs);
  }

  static llvm::Value * createAbs(Code *code, llvm::Value *arg)
  {
      return code->getBuilder().CreateCall(code->getRealAbsFunction(), arg);
  }

  static llvm::Value * createLog(Code *code, llvm::Value *arg)
  {
      return code->getBuilder().CreateCall(code->getRealLogFunction(), arg);
  }

  static llvm::Value * createExp(Code *code, llvm::Value *arg)
  {
      return code->getBuilder().CreateCall(code->getRealExpFunction(), arg);
  }
};


}
}
}

#endif // BUILDER_HH
