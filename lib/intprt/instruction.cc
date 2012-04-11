#include "instruction.hh"

using namespace Fluc;
using namespace Fluc::Intprt;



/* ********************************************************************************************* *
 * Implementation of ByteCode:
 * ********************************************************************************************* */
ByteCode::ByteCode()
  : code(), max_stack_size(0)
{
  // Pass...
}



ByteCode::ByteCode(const ByteCode &other)
  : code(other.code), max_stack_size(other.max_stack_size)
{
  // Pass...
}



const ByteCode &
ByteCode::operator<< (const Instruction &instruction)
{
  this->code.push_back(instruction);
  return *this;
}


const ByteCode &
ByteCode::operator<<(const ByteCode &other)
{
  for(ByteCode::const_iterator iter=other.begin(); iter!=other.end(); iter++)
  {
    (*this) << *iter;
  }

  // Update max_stack_size;
  this->max_stack_size = std::max(this->max_stack_size, other.max_stack_size);

  return *this;
}


bool
ByteCode::check()
{
  size_t current_stack_size = 0;
  size_t maximum_stack_size = 0;

  for (ByteCode::iterator inst = this->begin(); inst != this->end(); inst++)
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
    }
  }

  // Update max_stack_size:
  this->max_stack_size = std::max(this->max_stack_size, maximum_stack_size);

  // check if current stack-size == 0 (ie. byte-code is balanced):
  return 0 == current_stack_size;
}


size_t
ByteCode::getMinStackSize() const
{
  return this->max_stack_size;
}
