#ifndef __FLUC_EVALUATE_LLVM_COMPILER_HH__
#define __FLUC_EVALUATE_LLVM_COMPILER_HH__

#include "code.hh"
#include "assembler.hh"
#include "builder.hh"
#include "eval/compilercommon.hh"

#include <ginac.h>
#include <eigen3/Eigen/Eigen>
#include "ginacsupportforeigen.hh"


namespace Fluc {
namespace Evaluate {
namespace LLVM {


template <class InType, class OutType=InType>
class Compiler
    : public Evaluate::CompilerCommon<InType, OutType>
{
protected:
  Code *code;

  /**
   * Maps a GiNaC symbol to an index of the input vector.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;


public:
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(0), index_table(index_table)
  {
    // Pass...
  }


  /**
   * Resets the compiler.
   */
  void setCode(Code *code)
  {
    this->code = code;
  }


  /**
   * Performs some optimizations on the LLVM IR.
   */
  virtual void finalize(size_t level=0)
  {
    // create "return void" instruction:
    code->getBuilder().CreateRetVoid();

    // First, verify function:
    /*llvm::verifyModule(*this->code->getModule());
    this->code->getModule()->dump();*/

    // Compile function:
    this->code->compile(level);
  }


  /**
   * Compiles expression, the result of this expression will be stored at the given index in the
   * output vector.
   */
  virtual void compileExpressionAndStore(GiNaC::ex &expression, size_t index)
  {
    Assembler<typename OutType::Scalar> assembler(this->code, this->index_table);
    expression.accept(assembler);
    llvm::Value *value = assembler.popValue();

    Builder<typename OutType::Scalar>::createStore(this->code, value, index);
  }
};


}
}
}

#endif // COMPILER_HH
