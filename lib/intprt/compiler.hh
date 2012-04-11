#ifndef __FLUC_INTPRT_COMPILER_HH__
#define __FLUC_INTPRT_COMPILER_HH__

#include "instruction.hh"
#include <map>
#include <ginac/ginac.h>
#include "ast/model.hh"
#include "assembler.hh"
#include "pass.hh"


namespace Fluc {
namespace Intprt {


/**
 * Compiles and optimizes some GiNaC expressions into byte-code, that can be evaluated by an
 * @c Interpreter.
 *
 * @ingroup intprt
 */
template <class InType, class OutType=InType>
class Compiler
{
protected:
  /**
   * Holds the code to be executed.
   */
  ByteCode code;

  /**
   * Maps a GiNaC symbol to an index of the input vector.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;


public:
  /**
   * Constructs an compiler with some empty byte-code.
   *
   * Once all code is compiled, you can get it using @c getCode. You can also reset a compiler
   * using @c reset.
   */
  Compiler()
    : code(), index_table()
  {
    // Pass...
  }


  /**
   * Constructs a compiler using the given index-table.
   */
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(), index_table(index_table)
  {
    // Pass...
  }


  /**
   * Constructs a compiler using the symbol->index mapping of the given @c Ast::Model.
   */
  Compiler(const Ast::Model &model)
  {
    // Associate species symbol with its index in model:
    for (size_t i=0; i<model.numSpecies(); i++) {
      this->index_table[model.getSpecies(i)->getSymbol()] = i;
    }
  }


  /**
   * Resets the compiler, deletes all code.
   */
  void reset()
  {
    this->code = ByteCode();
  }


  /**
   * Returns the compiled code.
   */
  const ByteCode &getCode() const
  {
    return this->code;
  }


  /**
   * Compiles a single GiNaC expression into byte-code.
   */
  void compileExpression(GiNaC::ex &expression)
  {
    AssemblerByTable assembler(*(this->code), this->index_table);
    expression.accept(assembler);
  }


  /**
   * Compiles a single STORE statement.
   */
  void compileStore(size_t index)
  {
    this->code << Instruction(Instruction::STORE, index);
  }


  /**
   * Compiles expression and creates a STORE instruction, that will store the value of the
   * expression at the given index in the output-vector during evaluation.
   */
  void compileExpressionAndStore(GiNaC::ex &expression, size_t index)
  {
    this->compileExpression(expression);
    this->compileStore(index);
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
   * Performs some optimizations on the byte-code.
   */
  void optimize(size_t level)
  {
    /*
     * In general, for the byte-code interpreter: The shortest code is the fastest!
     *
     * There some simple passes, trying to optimize the byte-code for a faster execution.
     */

    /*
     * The first pass does not optimize any code here, it just pepares the representation for
     * the second pass.
     *
     * The first pass (ImmediateValueRHSPass) tryes to swap the operands of kommutative operations
     * like ADD and MUL if the LHS (left) operand is a constant (a PUSH instruction) and the RHS
     * value is not.
     *
     * The second pass (ImmediatValuePass) encodes the RHS operand as an immediate value
     * (a value passed along with the instruction itself) if it is a constant (a PUSH instruction).
     * This pass reduces the number of instructions and the number of stack operations, since
     * operations with immediate values are performed in-place on the stack.
     */
    ImmediateValueRHSPass imm_rhs_pass;
    ImmediateValuePass    imm_pass;
    RemoveUnitsPass       units_pass;

    if (level >= 1) {
      imm_rhs_pass.apply(this->code);
      imm_pass.apply(this->code);
      units_pass.apply(this->code);

      // Update stack-size:
      this->code.check();
    }
  }
};


}
}

#endif // __FLUC_INTPRT_COMPILER_HH__
