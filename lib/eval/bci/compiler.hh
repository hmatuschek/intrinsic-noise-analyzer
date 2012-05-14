#ifndef __FLUC_EVALUATE_BCI_COMPILER_HH__
#define __FLUC_EVALUATE_BCI_COMPILER_HH__

#include "code.hh"
#include <map>
#include <ginac/ginac.h>
#include "ginacsupportforeigen.hh"
#include "ast/model.hh"
#include "assembler.hh"
#include "pass.hh"
#include "utils/cputime.hh"
#include "utils/logger.hh"


namespace Fluc {
namespace Evaluate {
namespace bci {


/**
 * Compiles and optimizes some GiNaC expressions into byte-code, that can be evaluated by an
 * @c Interpreter.
 *
 * @ingroup eval
 */
template <class InType, class OutType=InType>
class Compiler
{
protected:
  /**
   * Holds the code to be executed.
   */
  Code *code;

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
    : code(0), index_table()
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
  void setCode(Code *code)
  {
    this->code = code;
  }


  /**
   * Compiles expression and creates a STORE instruction, that will store the value of the
   * expression at the given index in the output-vector during evaluation.
   */
  void compileExpressionAndStore(GiNaC::ex &expression, size_t index)
  {
    Assembler assembler(this->code, this->index_table);
    expression.accept(assembler);
    *(this->code) << Instruction(Instruction::STORE, index);
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
  void finalize(size_t level=0)
  {
    this->code->check();
    Utils::CpuTime clock; clock.start();
    /*
     * In general, for the byte-code interpreter: The shortest code is the fastest!
     *
     * There some simple passes, trying to optimize the byte-code for a faster execution.
     */
    ImmediateValueRHSPass imm_rhs_pass;
    ImmediateValuePass    imm_pass;
    RemoveUnitsPass       units_pass;
    ConstantFoldingPass   const_pass;

    if (level >= 1) {
      /* The first pass does not optimize any code here, it just pepares the representation for
       * the second pass.
       *
       * The first pass (ImmediateValueRHSPass) tries to swap the operands of kommutative operations
       * like ADD and MUL if the LHS (left) operand is a constant (a PUSH instruction) and the RHS
       * value is not. */
      imm_rhs_pass.apply(*code);

      /* The second pass (ImmediatValuePass) encodes the RHS operand as an immediate value
       * (a value passed along with the instruction itself) if it is a constant (a PUSH instruction).
       * This pass reduces the number of instructions and the number of stack operations, since
       * operations with immediate values are performed in-place on the stack. */
      imm_pass.apply(*code);

      /* This pass removes unit-operations like X*1 or X+0 etc. */
      units_pass.apply(*code);

      /* This pass evaluates function calls of constant values like ln(2) etc. */
      const_pass.apply(*code);

      // Update stack-size:
      this->code->check();
    }

    Utils::Message message(LOG_MESSAGE(Utils::Message::DEBUG));
    message << "Optimized byte-code in " << clock.stop() << "s.";
    Utils::Logger::get().log(message);
  }
};


}
}
}

#endif
