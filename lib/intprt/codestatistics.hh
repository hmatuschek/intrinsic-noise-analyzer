#ifndef __FLUC_INTPRT_CODESTATISTICS_HH__
#define __FLUC_INTPRT_CODESTATISTICS_HH__

#include "instruction.hh"

#include <iostream>
#include <vector>
#include <map>


namespace Fluc {
namespace Intprt {


/**
 * This simple class collects some statistics about the given byte-code.
 *
 * @ingroup intprt
 */
class CodeStatistics
{
protected:
  /**
   * Holds the counters for the instruction opcodes.
   */
  std::map<Instruction::OpCode, size_t> instruction_count;

public:
  /**
   * Collects the statistics about the given code.
   */
  CodeStatistics(const std::vector<Instruction> &code);

  /**
   * Returns the total number of instructions.
   */
  size_t numInstructions();

  /**
   * Returns the number of instructions for the given opcode.
   */
  size_t numInstructions(Instruction::OpCode opcode);

  /**
   * Simply dumps the statistics into the given stream.
   */
  void dump(std::ostream &stream);


protected:
  /**
   * Internal method to update the statistics.
   */
  void handleInstruction(const Instruction &instruction);
};


}
}

#endif // __FLUC_INTPRT_CODESTATISTICS_HH__
