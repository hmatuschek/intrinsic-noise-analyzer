#include "code.hh"

using namespace Fluc;
using namespace Fluc::Evaluate::bci;



/* ********************************************************************************************* *
 * Implementation of Code:
 * ********************************************************************************************* */
Code::Code()
  : code(), max_stack_size(0)
{
  // Pass...
}



Code::Code(const Code &other)
  : code(other.code), max_stack_size(other.max_stack_size)
{
  // Pass...
}



const Code &
Code::operator<< (const Instruction &instruction)
{
  this->code.push_back(instruction);
  return *this;
}


const Code &
Code::operator<<(const Code &other)
{
  for(Code::const_iterator iter=other.begin(); iter!=other.end(); iter++)
  {
    (*this) << *iter;
  }

  // Update max_stack_size;
  this->max_stack_size = std::max(this->max_stack_size, other.max_stack_size);

  return *this;
}


bool
Code::check()
{
  size_t current_stack_size = 0;
  size_t maximum_stack_size = 0;

  for (Code::iterator inst = this->begin(); inst != this->end(); inst++)
  {
    switch(inst->opcode)
    {
    case Instruction::ADD:
    case Instruction::SUB:
    case Instruction::MUL:
    case Instruction::DIV:
    case Instruction::POW:
      // Binary operator needs at least two elements on the stack:
      if ((2 > current_stack_size && inst->valueImmediate) || (1 > current_stack_size && !inst->valueImmediate))
        return false;
      // reduces the effective stack-size by -1:
      current_stack_size -= 1;
      break;

    case Instruction::STORE:
      // STORE needs at least one element on the stack:
      if (1 > current_stack_size)
        return false;
      // Reduces the effective stack-size by -1:
      current_stack_size -= 1;
      break;

    case Instruction::LOAD:
    case Instruction::PUSH:
      // May increase the maximum stack-size by 1:
      if (maximum_stack_size == current_stack_size)
        maximum_stack_size += 1;
      current_stack_size += 1;
      break;

    case Instruction::CALL:
      // CALL instructions needs at least on element on the stack:
      switch ( Instruction::FunctionCode(inst->value.asIndex) ) {
      case Instruction::FUNCTION_ABS:
      case Instruction::FUNCTION_LOG:
      case Instruction::FUNCTION_EXP:
        if (1 > current_stack_size)
          return false;
        break;
      }
      // Call instructions do not change the stack-size.
      break;
    }
  }

  // Update max_stack_size:
  this->max_stack_size = std::max(this->max_stack_size, maximum_stack_size);

  // check if current stack-size == 0 (i.e. bytecode is balanced):
  return 0 == current_stack_size;
}


size_t
Code::getMinStackSize() const
{
  return this->max_stack_size;
}


void
Code::dump(std::ostream &str)
{
  for (Code::iterator inst = this->begin(); inst != this->end(); inst++)
  {
    switch(inst->opcode)
    {
    case Instruction::ADD:
      str << "   ADD";
      if (inst->valueImmediate)
        str << "   " << inst->value.asComplex.real
            << " + " << inst->value.asComplex.imag << "j";
      str << std::endl;
      break;

    case Instruction::SUB:
      str << "   SUB";
      if (inst->valueImmediate)
        str << "   " << inst->value.asComplex.real
            << " + " << inst->value.asComplex.imag << "j";
      str << std::endl;
      break;

    case Instruction::MUL:
      str << "   MUL";
      if (inst->valueImmediate)
        str << "   " << inst->value.asComplex.real
            << " + " << inst->value.asComplex.imag << "j";
      str << std::endl;
      break;

    case Instruction::DIV:
      str << "   DIV";
      if (inst->valueImmediate)
        str << "   " << inst->value.asComplex.real
            << " + " << inst->value.asComplex.imag << "j";
      str << std::endl;
      break;

    case Instruction::POW:
      str << "   POW";
      if (inst->valueImmediate)
        str << "   " << inst->value.asComplex.real
            << " + " << inst->value.asComplex.imag << "j";
      str << std::endl;
      break;

    case Instruction::LOAD:
      str << "   LOAD  " << inst->value.asIndex << std::endl;
      break;

    case Instruction::STORE:
      str << "   STORE " << inst->value.asIndex << std::endl;
      break;

    case Instruction::PUSH:
      str << "   PUSH  " << inst->value.asComplex.real
          << " + " << inst->value.asComplex.imag << "j" << std::endl;
      break;

    case Instruction::CALL:
      switch (Instruction::FunctionCode(inst->value.asIndex)) {
      case Instruction::FUNCTION_ABS:
        str << "   ABS" << std::endl;
        break;

      case Instruction::FUNCTION_LOG:
        str << "   LOG" << std::endl;
        break;

      case Instruction::FUNCTION_EXP:
        str << "   EXP" << std::endl;
        break;
      }
    }
  }
}
