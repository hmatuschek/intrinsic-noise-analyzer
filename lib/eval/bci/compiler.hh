#ifndef __INA_EVAL_BCI_COMPILER_HH__
#define __INA_EVAL_BCI_COMPILER_HH__

#include "code.hh"
#include <map>
#include <ginac/ginac.h>
#include "../../ginacsupportforeigen.hh"
#include "../../ast/model.hh"
#include "assembler.hh"
#include "pass.hh"
#include "../../utils/cputime.hh"
#include "../../utils/logger.hh"
#include "../compilercommon.hh"


namespace iNA {
namespace Eval {
namespace bci {


/**
 * Compiles and optimizes some GiNaC expressions into byte-code, that can be evaluated by an
 * @c Interpreter.
 *
 * @ingroup bci
 */
template <class InType, class OutType=InType>
class Compiler : public CompilerCommon<InType, OutType>
{
protected:
  /** Holds the code to be executed. */
  Code *code;

  /** Maps a GiNaC symbol to an index of the input vector. */
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


  /** Constructs a compiler using the given index-table. */
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(), index_table(index_table)
  {
    // Pass...
  }


  /** Constructs a compiler using the symbol->index mapping of the given @c Ast::Model. */
  Compiler(const Ast::Model &model)
  {
    // Associate species symbol with its index in model:
    for (size_t i=0; i<model.numSpecies(); i++) {
      this->index_table[model.getSpecies(i)->getSymbol()] = i;
    }
  }


  /** Resets the compiler, deletes all code.  */
  void setCode(Code *code)
  {
    this->code = code;
  }


  /** Compiles expression and creates a STORE instruction, that will store the value of the
   * expression at the given index in the output-vector during evaluation.  */
  virtual void compileExpressionAndStore(const GiNaC::ex &expression, size_t index)
  {
    Assembler assembler(this->code, this->index_table);
    expression.accept(assembler);
    *(this->code) << Instruction(Instruction::STORE, index);
  }


  /** Performs some optimizations on the byte-code. */
  virtual void finalize(size_t level=0)
  {
    this->code->check();

    /* In general, for the byte-code interpreter: The shortest code is the fastest!
     * There some simple passes, trying to optimize the byte-code for a faster execution. */
    ImmediateValueRHSPass   imm_rhs_pass;
    ImmediateValuePass      imm_pass;
    ConstantPropagation     const_prop;
    RemoveUnitsPass         units_pass;
    ConstantFoldingPass     const_pass;
    //IPowPass              ipow_pass;
    ZeroStorePass           zero_pass;
    InstructionCanonization canon_pass;

    PassManager manager;

    /* The first pass does not optimize any code here, it just pepares the representation for
     * the second pass.
     *
     * The first pass (ImmediateValueRHSPass) tries to swap the operands of kommutative operations
     * like ADD and MUL if the LHS (left) operand is a constant (a PUSH instruction) and the RHS
     * value is not. */
    manager.addPass(&imm_rhs_pass);

    /* The second pass (ImmediatValuePass) encodes the RHS operand as an immediate value
     * (a value passed along with the instruction itself) if it is a constant (a PUSH instruction).
     * This pass reduces the number of instructions and the number of stack operations, since
     * operations with immediate values are performed in-place on the stack. */
    manager.addPass(&imm_pass);

    /* This pass propagates constants to the outermost left position. */
    //manager.addPass(&const_prop);

    /* This pass removes unit-operations like X*1 or X+0 etc. */
    manager.addPass(&units_pass);

    /* This pass evaluates function calls of constant values like ln(2) etc. */
    manager.addPass(&const_pass);

    /* This pass replaces POW instructions with IPOW if latter is more efficient. */
    //manager.addPass(&ipow_pass);

    /* This pass replaces "PUSH 0, STORE IDX" with "STORE_ZERO IDX" */
    manager.addPass(&zero_pass);

    /* Canonizes instructions. */
    //manager.addPass(&canon_pass);

    if (level >= 1) {
      Utils::CpuTime clock; clock.start();
      size_t old_code_size  = code->getCodeSize();

      // Apply passes.
      manager.apply(*code);

      // Update stack-size:
      this->code->check();

      Utils::Message message(LOG_MESSAGE(Utils::Message::DEBUG));
      message << "Optimized byte-code in " << clock.stop() << "s: "
              << "code-size: " << old_code_size << " -> " << code->getCodeSize() << " ("
              << int(100*(double(old_code_size)-code->getCodeSize())/old_code_size) << "%)";
      Utils::Logger::get().log(message);
    }

    //code->dump(std::cerr);
  }
};


}
}
}

#endif
