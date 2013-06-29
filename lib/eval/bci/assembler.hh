#ifndef __INA_EVAL_BCI_ASSEMBLER_HH__
#define __INA_EVAL_BCI_ASSEMBLER_HH__

#include "../../ast/ast.hh"
#include "code.hh"


namespace iNA {
namespace Eval {
namespace bci {


/**
 * Implements the assembler for the ByteCode from GiNaC expressions.
 *
 * This class is used by the @c Compiler class to assemble the byte-code from GiNaC expressions.
 *
 * @ingroup bci
 */
class Assembler
    : public GiNaC::visitor, public GiNaC::numeric::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::symbol::visitor, public GiNaC::power::visitor,
    public GiNaC::function::visitor, public GiNaC::basic::visitor
{
protected:
  /** Holds a reference to the list of instructions to assemble. New instructions are appended. */
  Code *code;

  /** Holds the translation table symbol->index. */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &symbol_table;

  /** Holds the translation-table GiNaC Function serial -> Function Code: */
  std::map<unsigned, Instruction::FunctionCode> function_codes;


public:
  /** Constructs a new assembler.
   * @param code Speciefies the list of instructions to be extended.
   * @param symbol_table Specifies the mapping from GiNaC symbols to indices of the input vector. */
  Assembler(Code *code, std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &symbol_table);

  /** Handles constant numerical (float) values. */
  void visit(const GiNaC::numeric &value);
  /** Handles a variable (symbol). */
  void visit(const GiNaC::symbol &symbol);
  /** First, processes all summands and finally assembles sum. */
  void visit(const GiNaC::add &sum);
  /** First, processes all factors and finally assembles product. */
  void visit(const GiNaC::mul &prod);
  /** Handles powers. */
  void visit(const GiNaC::power &pow);
  /** Handles function calls. */
  void visit(const GiNaC::function &function);
  /** Handles all unhandled expression parts -> throws an exception. */
  void visit(const GiNaC::basic &basic);
};


}
}
}


#endif
