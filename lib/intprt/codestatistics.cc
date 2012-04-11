#include "codestatistics.hh"
#include <iostream>

using namespace Fluc;
using namespace Fluc::Intprt;


CodeStatistics::CodeStatistics(const std::vector<Instruction> &code)
  : instruction_count()
{
  for (size_t i=0; i<code.size(); i++)
  {
    this->handleInstruction(code[i]);
  }
}


void
CodeStatistics::handleInstruction(const Instruction &instruction)
{
  this->instruction_count[instruction.opcode]++;
}


size_t
CodeStatistics::numInstructions()
{
  size_t count = 0;

  for (std::map<Instruction::OpCode, size_t>::iterator iter=this->instruction_count.begin();
       iter != this->instruction_count.end(); iter++)
  {
    count += iter->second;
  }

  return count;
}


size_t
CodeStatistics::numInstructions(Instruction::OpCode opcode)
{
  std::map<Instruction::OpCode, size_t>::iterator item = this->instruction_count.find(opcode);

  if (this->instruction_count.end() == item)
  {
    return 0;
  }

  return item->second;
}


void
CodeStatistics::dump(std::ostream &stream)
{
  stream << "ByteCode statistics: " << std::endl
         << "  add:    " << this->numInstructions(Instruction::ADD) << std::endl
         << "  sub:    " << this->numInstructions(Instruction::SUB) << std::endl
         << "  mul:    " << this->numInstructions(Instruction::MUL) << std::endl
         << "  div:    " << this->numInstructions(Instruction::DIV) << std::endl
         << "  pow:    " << this->numInstructions(Instruction::POW) << std::endl
         << "  load:   " << this->numInstructions(Instruction::LOAD) << std::endl
         << "  store:  " << this->numInstructions(Instruction::STORE) << std::endl
         << "  push:   " << this->numInstructions(Instruction::PUSH) << std::endl
         << "  TOTOAL: " << this->numInstructions() << std::endl;
}
