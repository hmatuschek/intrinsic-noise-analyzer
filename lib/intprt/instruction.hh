#ifndef __FLUC_INTPRT_INSTRUCTION_HH__
#define __FLUC_INTPRT_INSTRUCTION_HH__

#include <vector>
#include <cstdlib>
#include <complex>


namespace Fluc {
namespace Intprt {

/**
 * Represents a single instance of a ByteCode instruction.
 *
 * @ingroup intprt
 */
class Instruction
{
public:
  /**
   * Defines all avaliable op-codes.
   */
  typedef enum {
    ADD,    ///< Pops rhs & lhs values from stack and pushes the back the sum.
    SUB,    ///< Pops rhs & lhs values from stack and pushes the back the difference.
    MUL,    ///< Pops rhs & lhs values from stack and pushes the back the product.
    DIV,    ///< Pops rhs & lhs values from stack and pushes the back the quotient.
    POW,    ///< Pops rhs & lhs values from stack and pushes the back the result.
    LOAD,   ///< Loads a value from the input vector and pushes it on the stack.
    STORE,  ///< Pops a value from the stack and stores it into the output vector.
    PUSH    ///< Pushes an immediate value on the stack.
  } OpCode;


public:
  /**
   * Holds the op-code.
   */
  OpCode opcode;

  /**
   * An immediate value:
   */
  union {
    double asValue;                 ///< The immediate value as a double value.
    //std::complex<double> asComplex; ///< The immediate value as a complex-double value.
    size_t asIndex;                 ///< The immediate value as a integer index.
  } value;

  /**
   * Flag to indicate immediate value.
   */
  bool valueImmediate;


public:
  /**
   * Constructs a simple instruction without immediate values.
   */
  Instruction(OpCode opcode)
    : opcode(opcode), valueImmediate(false)
  {
    // Pass...
  }

  /**
   * Constructs an instruction with index immediate value.
   */
  Instruction(OpCode opcode, size_t value)
    : opcode(opcode), valueImmediate(true)
  {
    this->value.asIndex = value;
  }

  /**
   * Constructs an instruction with a double immediate value.
   */
  Instruction(OpCode opcode, double value)
    : opcode(opcode), valueImmediate(true)
  {
    this->value.asValue = value;
  }
};




/**
 * Represents a series of instructions; The byte-code.
 *
 * @ingroup intprt
 */
class ByteCode
{
public:
  /**
   * Defines the iterator type over the instructions.
   */
  typedef std::vector<Instruction>::iterator iterator;

  /**
   * Defines the iterator type over the instructions.
   */
  typedef std::vector<Instruction>::const_iterator const_iterator;


protected:
  /**
   * Holds the actual byte-code instructions.
   */
  std::vector<Instruction> code;

  /**
   * Max stack size.
   */
  size_t max_stack_size;


public:
  /**
   * Constructs an empty byte-code container.
   */
  ByteCode();

  /**
   * Copy constructor.
   */
  ByteCode(const ByteCode &other);

  /**
   * Appends the given byte-code to this byte-code.
   */
  const ByteCode &operator<< (const ByteCode &other);

  /**
   * Appends a given instruction to the byte-code.
   */
  const ByteCode &operator<< (const Instruction &instruction);

  /**
   * Checks the byte-code for constistency and determines the minimum stack-size needed to
   * evaluate the code.
   *
   * @returns False if the byte-code is not balanced.
   */
  bool check();


  /**
   * Returns the minimum stack-size required to execute this code.
   *
   * @note You need to call check() to update the minimum required stack-size once, the byte-code
   *       was modified.
   */
  size_t getMinStackSize() const;


  /**
   * Retunrs an iterator to the first instruction of the byte-code.
   */
  inline iterator begin()
  {
    return this->code.begin();
  }

  /**
   * Returns an iterator pointing right after the last instruction.
   */
  inline iterator end()
  {
    return this->code.end();
  }


  /**
   * Retunrs an iterator to the first instruction of the byte-code.
   */
  inline const_iterator begin() const
  {
    return this->code.begin();
  }

  /**
   * Returns an iterator pointing right after the last instruction.
   */
  inline const_iterator end() const
  {
    return this->code.end();
  }


  /**
   * Retruns a reference to the vector of instructions.
   *
   * @deprecated Just there to be compatible with "old" Assembler...
   */
  std::vector<Instruction> &operator* ()
  {
    return this->code;
  }
};


}
}
#endif // __FLUC_INTPRT_INSTRUCTION_HH__
