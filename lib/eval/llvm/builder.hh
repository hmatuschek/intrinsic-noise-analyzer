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
  /** Creates a constant index expression. */
  static llvm::Value *createIndex(Code *code, size_t index);

  /** Creates a constrant floating-point expression. (in this case, a double). */
  static llvm::Value *createConstant(Code *code, const GiNaC::numeric &value);

  static void createStore(Code *code, llvm::Value *value, size_t index);

  static llvm::Value *createLoad(Code *code, size_t index);

  static llvm::Value *createAdd(Code *code, llvm::Value *lhs, llvm::Value *rhs);

  static llvm::Value *createMul(Code *code, llvm::Value *lhs, llvm::Value *rhs);

  static llvm::Value *createPow(Code *code, llvm::Value *lhs, llvm::Value *rhs);

  static llvm::Value * createAbs(Code *code, llvm::Value *arg);

  static llvm::Value * createLog(Code *code, llvm::Value *arg);

  static llvm::Value * createExp(Code *code, llvm::Value *arg);
};


}
}
}

#endif // BUILDER_HH
