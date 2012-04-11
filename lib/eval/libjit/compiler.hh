#ifndef __FLUC_EVALUATE_LIBJIT_COMPILER_HH__
#define __FLUC_EVALUATE_LIBJIT_COMPILER_HH__

#include "code.hh"
#include "exception.hh"
#include "assembler.hh"

#include <map>
#include <ginac/ginac.h>
#include "eigen3/Eigen/Eigen"
#include "ginacsupportforeigen.hh"


namespace Fluc {
namespace Evaluate {
namespace libjit {



/**
 * Compiles GiNaC expressions into libjit IR.
 *
 * @ingroup eval
 */
template <class InType, class OutType=InType>
class Compiler
{
protected:
  /**
   * Holds a reference to the code.
   */
  Code *code;

  /**
   * Maps a GiNaC symbol to an index of the input vector.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;


public:
  /**
   * Constructs a compiler using the given code and index table.
   */
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(0), index_table(index_table)
  {
    // Pass...
  }


  /**
   * Compiles a single GiNaC expression and associates its value with the given index in the
   * output vector.
   */
  void compileExpressionAndStore(GiNaC::ex &expression, size_t index)
  {
    // First assemble expression using assembler:
    Assembler<typename OutType::Scalar> assembler(this->code, this->index_table);
    expression.accept(assembler);

    // Get last value from stack:
    jit_value_t value = assembler.popValue();

    // Then create store instruction:
    jit_insn_store_elem(this->code->getFunction(),
                        jit_value_get_param(this->code->getFunction(), 1),
                        IndexBuilder::create(this->code->getFunction(), index),
                        value);
  }


  /**
   * Compiles a vector of expressions.
   *
   * The value of the i-th expression will be stored at the i-th index in the output-vector
   * during evaluation.
   */
  void compileVector(Eigen::VectorXex &vector)
  {
    for (int index = 0; index < vector.rows(); index++)
    {
      this->compileExpressionAndStore(vector(index), index);
    }
  }


  /**
   * Compiles a matrix of expressions, that will evaluate to an @c Eigen::MatrixXd of the same
   * shape.
   */
  void compileMatrix(Eigen::MatrixXex &matrix)
  {
    for (int i=0; i<matrix.rows(); i++)
    {
      for (int j=0; j<matrix.cols(); j++)
      {
        size_t index = 0;
        if (OutType::Flags & Eigen::RowMajorBit)
          index = j + matrix.cols()*i;
        else
          index = i + matrix.rows()*j;

        this->compileExpressionAndStore(matrix(i,j), index);
      }
    }
  }


  /**
   * Sets the code, starts compilation.
   */
  void setCode(Code *code)
  {
    this->code = code;
    jit_context_build_start(jit_function_get_context(code->getFunction()));
  }


  /**
   * Finalizes the compilation process.
   *
   * Needs to be called before the code can be executed.
   */
  void finalize(size_t optlevel=0)
  {
    // Close function-body
    jit_insn_return(this->code->getFunction(), 0);
    // Compile function
    jit_function_compile(this->code->getFunction());
    // Finalize JIT context:
    jit_context_build_end(jit_function_get_context(this->code->getFunction()));

    // Now dump the code:


    // Done.
    this->code = 0;
  }
};


}
}
}
#endif
