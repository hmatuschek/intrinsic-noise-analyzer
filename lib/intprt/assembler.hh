#ifndef __FLUC_INTPRT_ASSEMBLER_HH__
#define __FLUC_INTPRT_ASSEMBLER_HH__

#include "ast/ast.hh"
#include "interpreter.hh"


namespace Fluc {
namespace Intprt {


/**
 * Base class of expression assemblers for the ByteCode.
 *
 * This class is pure-virtual, the method @c resolveSymbol needs to be implemented by any
 * sub-class to resolve a GiNaC::symbol to an index of the input vector.
 *
 * @ingroup intprt
 */
class ExpressionAssembler
    : public GiNaC::visitor, public GiNaC::numeric::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::symbol::visitor, public GiNaC::power::visitor
{
protected:
  /**
   * Holds a reference to the list of instructions to assemble. New instructions are appended.
   */
  std::vector<Instruction> &code;


public:
  /**
   * Constructs a new assembler.
   *
   * @param code Speciefies the list of instructions to be extended.
   */
  ExpressionAssembler(std::vector<Instruction> &code);

  /**
   * Pure virtual method, needs to be implemented by sub-classes to resolve @c GiNaC::symbol
   * instances to indices of the input vector.
   */
  virtual size_t resolveSymbol(const GiNaC::symbol &symbol) = 0;

  /**
   * Handles constant numerical (float) values.
   */
  void visit(const GiNaC::numeric &value);

  /**
   * Handles a variable (symbol).
   */
  void visit(const GiNaC::symbol &symbol);

  /**
   * First, processes all summands and finally assembles sum.
   */
  void visit(const GiNaC::add &sum);

  /**
   * First, processes all factors and finally assembles product.
   */
  void visit(const GiNaC::mul &prod);

  /**
   * Handles powers.
   */
  void visit(const GiNaC::power &pow);
};



/**
 * Assembles @c GiNaC::ex expressions into byte-code while resolving symbols using a table given
 * the the constructor.
 *
 * @ingroup intprt
 */
class AssemblerByTable : public ExpressionAssembler
{
protected:
  /**
   * Holds the translation table symbol->index.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> symbol_table;


public:
  /**
   * Constructs an @c ExpressionAssembler that resolves the symbols of the expression to an
   * index usign @c symbol_table.
   *
   * @note This assembler links the code to the specified vector instance.
   */
  AssemblerByTable(std::vector<Instruction> &code,
                   std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &symbol_table);

  /**
   * Implements the symbol-resolution.
   */
  size_t resolveSymbol(const GiNaC::symbol &symbol);
};


}
}


#endif // __FLUC_INTPRT_ASSEMBLER_HH__
