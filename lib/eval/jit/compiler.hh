#ifndef __INA_EVALUATE_LLVM_COMPILER_HH__
#define __INA_EVALUATE_LLVM_COMPILER_HH__

#include "code.hh"
#include "assembler.hh"
#include "builder.hh"
#include "../compilercommon.hh"

#include <ginac/ginac.h>
#include <eigen3/Eigen/Eigen>
#include "../../ginacsupportforeigen.hh"


namespace iNA {
namespace Eval {
namespace jit {


/** Some type-independent parts of the compiler. */
class CompilerCore
{
public:
  CompilerCore(Code *code=0);

  /** Resets the compiler. */
  void setCode(Code *code);
  /** Performs some optimizations on the LLVM IR. */
  void finalize(size_t level);

protected:
  Code *code;
};


/** This class compiles GiNaC expressions into LLVM IR code, that can then be JIT compiled into
 * native machine-code to be executed efficiently.
 * @ingroup jit */
template <class InType, class OutType=InType>
class Compiler :
    public CompilerCore,
    public Eval::CompilerCommon<InType, OutType>
{
public:
  /** Constructs a compiler with the given index-table.
   * @param index_table Specifies the mapping from a GiNaC symbol (state-variable) to an index
   *        of the state-vector (input vector). */
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : CompilerCore(0), index_table(index_table)
  {
    // Pass...
  }

  /** Finalizes the code (optionally optimizes it). */
  virtual void finalize(size_t level=0) {
    CompilerCore::finalize(level);
  }

  /** Compiles expression, the result of this expression will be stored at the given index in the
   * output vector. */
  virtual void compileExpressionAndStore(const GiNaC::ex &expression, size_t index)
  {
    Assembler<typename OutType::Scalar> assembler(this->code, this->index_table);
    expression.accept(assembler);
    llvm::Value *value = assembler.popValue();

    Builder<typename OutType::Scalar>::createStore(this->code, value, index);
  }

protected:
  /** Maps a GiNaC symbol to an index of the input vector. */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;
};


}
}
}

#endif // COMPILER_HH
