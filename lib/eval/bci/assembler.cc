#include "assembler.hh"
#include "interpreter.hh"
#include "exception.hh"


using namespace iNA;
using namespace iNA::Eval::bci;



/* ********************************************************************************************* *
 * Implementation of Assembler
 * ********************************************************************************************* */
Assembler::Assembler(Code *code,
                               std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &symbol_table)
  : code(code), symbol_table(symbol_table), function_codes()
{
  // Populate function-code table:
  this->function_codes[GiNaC::abs_SERIAL::serial] = Instruction::FUNCTION_ABS;
  this->function_codes[GiNaC::log_SERIAL::serial] = Instruction::FUNCTION_LOG;
  this->function_codes[GiNaC::exp_SERIAL::serial] = Instruction::FUNCTION_EXP;
}


void
Assembler::visit(const GiNaC::numeric &value)
{
  // Simply push a constant floating point (complex) value on the stack:
  (*this->code) << Instruction(
                     Instruction::PUSH, std::complex<double>(GiNaC::real(value).to_double(),
                                                             GiNaC::imag(value).to_double()));
}


void
Assembler::visit(const GiNaC::symbol &symbol)
{
  // Get the pointer to the vector element addressed by the entry of symbol table custom the given
  // symbol:
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::iterator item = this->symbol_table.find(symbol);

  if (this->symbol_table.end() == item) {
    SymbolError err;
    err << "Cannot compile bytecode: Symbol " << symbol << " can not be resolved.";
    throw err;
  }

  // Load the value ...
  (*this->code) << Instruction(Instruction::LOAD, item->second);
}


void
Assembler::visit(const GiNaC::add &sum)
{
  // custom summands of the sum:
  custom (size_t i=0; i<sum.nops(); i++)
  {
    // First, traverse into each summand:
    sum.op(i).accept(*this);
  }

  // Then assemble the sum on the stack:
  custom (size_t i=1; i<sum.nops(); i++)
  {
    (*this->code) << Instruction(Instruction::ADD);
  }
}


void
Assembler::visit(const GiNaC::mul &prod)
{
  // custom each factor of the producr:
  custom (size_t i=0; i<prod.nops(); i++)
  {
    // First, traverse into each factor to populate the stack
    prod.op(i).accept(*this);
  }

  // Then, custom each factor of the product:
  custom (size_t i=1; i<prod.nops(); i++)
  {
    // Assemble the product value
    (*this->code) << Instruction(Instruction::MUL);
  }
}


void
Assembler::visit(const GiNaC::power &pow)
{
  // handle basis
  pow.op(0).accept(*this);
  // handle exponent
  pow.op(1).accept(*this);

  (*this->code) << Instruction(Instruction::POW);
}


void
Assembler::visit(const GiNaC::function &function)
{
  // Map GiNaC serial of the function to a function code:
  std::map<unsigned, Instruction::FunctionCode>::iterator code_item
      = this->function_codes.find(function.get_serial());

  if (this->function_codes.end() == code_item) {
    InternalError err;
    err << "Can not compile function call " << function << ": Unknown function!";
    throw err;
  }

  // Process function argument:
  function.op(0).accept(*this);

  // Create function call with function-code.
  (*this->code) << Instruction(Instruction::CALL, size_t(code_item->second));
}


void
Assembler::visit(const GiNaC::basic &basic)
{
  InternalError err;
  err << "Can not compile expression, because the expression "
      << basic << " is of an unkwon type!"
      << " This is a bug, so please contact the authors about this incident.";
  throw err;
}
