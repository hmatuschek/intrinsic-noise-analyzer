#include "assembler.hh"
#include "interpreter.hh"

using namespace Fluc;
using namespace Fluc::Intprt;



/* ********************************************************************************************* *
 * Implementation of ExpressionAssembler (base-class)
 * ********************************************************************************************* */
ExpressionAssembler::ExpressionAssembler(std::vector<Instruction> &code)
  : code(code)
{
  // Pass...
}


void
ExpressionAssembler::visit(const GiNaC::numeric &value)
{
  // Simply push a constant floating point value on the stack:
  this->code.push_back(Instruction(Instruction::PUSH, value.to_double()));
}


void
ExpressionAssembler::visit(const GiNaC::symbol &symbol)
{
  // Simply resolve the symbol (returns a index to a variable)
  size_t index = this->resolveSymbol(symbol);

  // Load the value ...
  this->code.push_back(Instruction(Instruction::LOAD, index));
}


void
ExpressionAssembler::visit(const GiNaC::add &sum)
{
  // For summands of the sum:
  for (size_t i=0; i<sum.nops(); i++)
  {
    // First, traverse into each summand:
    sum.op(i).accept(*this);
  }

  // Then assemble the sum on the stack:
  for (size_t i=1; i<sum.nops(); i++)
  {
    this->code.push_back(Instruction(Instruction::ADD));
  }
}


void
ExpressionAssembler::visit(const GiNaC::mul &prod)
{
  // For each factor of the producr:
  for (size_t i=0; i<prod.nops(); i++)
  {
    // First, traverse into each factor to populate the stack
    prod.op(i).accept(*this);
  }

  // Then, for each factor of the product:
  for (size_t i=1; i<prod.nops(); i++)
  {
    // Assemble the product value
    this->code.push_back(Instruction(Instruction::MUL));
  }
}


void
ExpressionAssembler::visit(const GiNaC::power &pow)
{
  // handle basis
  pow.op(0).accept(*this);
  // handle exponent
  pow.op(1).accept(*this);

  this->code.push_back(Instruction(Instruction::POW));
}




/* ********************************************************************************************* *
 * Implementation of AssemblerByIndex
 * ********************************************************************************************* */
AssemblerByTable::AssemblerByTable(std::vector<Instruction> &code,
                                   std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &symbol_table)
  : ExpressionAssembler(code), symbol_table(symbol_table)
{
  // Pass...
}


size_t
AssemblerByTable::resolveSymbol(const GiNaC::symbol &symbol)
{
  // Get the pointer to the vector element addressed by the entry of symbol table for the given
  // symbol:
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::iterator item = this->symbol_table.find(symbol);

  if (this->symbol_table.end() == item) {
    SymbolError err;
    err << "Cannot compile bytecode: Symbol " << symbol << " cannot be resolved.";
    throw err;
  }

  return item->second;
}
