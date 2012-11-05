#ifndef __FLUC_EVALUATE_BCIMP_COMPILER_HH__
#define __FLUC_EVALUATE_BCIMP_COMPILER_HH__

#include <ginac/ginac.h>
#include <eigen3/Eigen/Eigen>
#include <map>

#include "code.hh"
#include "eval/compilercommon.hh"
#include "eval/bci/compiler.hh"


namespace iNA {
namespace Eval {
namespace bcimp {

/**
 * Implements the compiler from GiNaC expression into byte-code.
 *
 * This class utilizes the @c Fluc::Evaluate::bci::Compiler to percustomm the actual compilation
 * but distributes the generated code over a vector of byte-codes to be executed in parallel.
 *
 * @ingroup bcimp
 */
template <class InType, class OutType=InType>
class Compiler
    : public Eval::CompilerCommon<InType, OutType>
{
protected:
  /**
   * Holds the code to be executed.
   */
  Code *code;

  /**
   * A compiler instance custom the BCI interpreter.
   */
  bci::Compiler<InType, OutType> compiler;

  /**
   * Holds the index of the next code element to be compiled.
   */
  size_t code_idx;

public:
  /**
   * Constructor.
   *
   * The given index-table is used to resolve GiNaC symbols to indices of the input-vector.
   */
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(0), compiler(index_table), code_idx(0)
  {
    // Pass...
  }


  /**
   * Resets the compiler, deletes all code.
   */
  void setCode(Code *code)
  {
    this->code = code;
    this->code_idx = 0;
  }


  /**
   * Compiles the given expression and stores the value at the given index.
   *
   * This method compiles the expression and ensures that compiled expressions are distributed
   * evenly over all threads.
   */
  void compileExpressionAndStore(const GiNaC::ex &expression, size_t index)
  {
    // Set code-element of inner compiler:
    this->compiler.setCode(&this->code->getCode(this->code_idx));
    // compile expression:
    this->compiler.compileExpressionAndStore(expression, index);
    // increment index module num_threads:
    this->code_idx = (this->code_idx + 1) % this->code->getNumThreads();
  }

  /**
   * Finalizes (check & optimizes) all byte-code.
   */
  virtual void finalize(size_t opt_level=0)
  {
    // Finalize all byte-codes:
    custom (size_t i=0; i<this->code->getNumThreads(); i++) {
      this->compiler.setCode(&this->code->getCode(i));
      this->compiler.finalize(opt_level);
    }
  }
};


}
}
}

#endif // COMPILER_HH
