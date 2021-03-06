#ifndef __INA_EVAL_BCI_CODE_HH__
#define __INA_EVAL_BCI_CODE_HH__

#include <vector>
#include <cstdlib>
#include <complex>


namespace iNA {
namespace Eval {
namespace bci {


/**
 * Represents a single instance of a ByteCode instruction.
 *
 * @ingroup bci
 */
class Instruction
{
public:
  /** Defines all avaliable op-codes. */
  typedef enum {
    ADD,    ///< Pops rhs & lhs values from stack and pushes the back the sum.
    SUB,    ///< Pops rhs & lhs values from stack and pushes the back the difference.
    MUL,    ///< Pops rhs & lhs values from stack and pushes the back the product.
    DIV,    ///< Pops rhs & lhs values from stack and pushes the back the quotient.
    POW,    ///< Pops rhs & lhs values from stack and pushes the back the result.
    IPOW,   ///< Pops lhs value from stack and takes the integer exponent stored as index immediate value.
    LOAD,   ///< Loads a value from the input vector and pushes it on the stack.
    STORE,  ///< Pops a value from the stack and stores it into the output vector.
    STORE_ZERO, ///< Stores a zero into the output vector.
    PUSH,   ///< Pushes an immediate value on the stack.
    CALL    ///< Calls a built-in function. @c FunctionCode.
  } OpCode;


  /** Defines the function-codes for all built-in functions. */
  typedef enum {
    FUNCTION_ABS,  /// < Function code for the absolute value "abs()".
    FUNCTION_LOG,  /// < Function code for the natural logarithm "log()".
    FUNCTION_EXP   /// < Function code for the exponential function "exp()".
  } FunctionCode;


public:
  /** Holds the op-code. */
  OpCode opcode;

  /** An immediate value. An imediate value allows to store values as RHS operands
   * together with the instruction. */
  union {
    struct {
      double real;       ///< Holds the real part of the complex value.
      double imag;       ///< Holds the imag part of the complex value.
    } asComplex;         ///< The immediate value as a complex-double value.
    size_t asIndex;      ///< The immediate value as a integer index.
  } value;

  /** Flag to indicate immediate value. */
  bool valueImmediate;


public:
  /** Constructs a simple instruction without immediate values. */
  Instruction(OpCode opcode)
    : opcode(opcode), valueImmediate(false)
  {
    // Pass...
  }

  /** Constructs an instruction with index immediate value. */
  Instruction(OpCode opcode, size_t value)
    : opcode(opcode), valueImmediate(true)
  {
    this->value.asIndex = value;
  }

  /** Constructs an instruction with a double immediate value. */
  Instruction(OpCode opcode, double value)
    : opcode(opcode), valueImmediate(true)
  {
    this->value.asComplex.real = value;
    this->value.asComplex.imag = 0.0;
  }

  /** Constructs an instruction with a complex immediate value. */
  Instruction(OpCode opcode, const std::complex<double> &value)
    : opcode(opcode), valueImmediate(true)
  {
    this->value.asComplex.real = value.real();
    this->value.asComplex.imag = value.imag();
  }

  /** Unpacks a complex immediate value of an instruction. */
  std::complex<double> immediateComplex() const {
    return std::complex<double>(value.asComplex.real, value.asComplex.imag);
  }

  /** Unpacks a real immediate value of an instruction. */
  double immediateReal() const {
    return value.asComplex.real;
  }
};




/**
 * Represents a series of instructions; The byte-code.
 *
 * @ingroup bci
 */
class Code
{
public:
  /** Defines the iterator type over the instructions. */
  typedef std::vector<Instruction>::iterator iterator;

  /** Defines the iterator type over the instructions. */
  typedef std::vector<Instruction>::const_iterator const_iterator;


protected:
  /** Holds the actual byte-code instructions. */
  std::vector<Instruction> code;

  /** Max stack size. */
  size_t max_stack_size;


public:
  /** Constructs an empty byte-code container. */
  Code(size_t num_threads=1);

  /** Copy constructor. */
  Code(const Code &other);

  /** Appends the given byte-code to this byte-code. */
  const Code &operator<< (const Code &other);

  /** Appends a given instruction to the byte-code. */
  const Code &operator<< (const Instruction &instruction);

  /** Checks the byte-code for constistency and determines the minimum stack-size needed to
   * evaluate the code.
   * @returns False if the byte-code is not balanced. */
  bool check();

  /** Dumps the code into the given stream. */
  void dump(std::ostream &str);

  /** Returns the minimum stack-size required to execute this code.
   * @note You need to call check() to update the minimum required stack size once, the bytecode
   *       was modified. */
  size_t getMinStackSize() const;

  /** Returns the number of instructions of the byte code. */
  size_t getCodeSize() const;

  /** Clears the bytecode. */
  void clear();

  /** Returns an iterator to the first instruction of the bytecode. */
  inline iterator begin() {
    return this->code.begin();
  }

  /** Returns an iterator pointing right after the last instruction. */
  inline iterator end() {
    return this->code.end();
  }

  /** Returns an iterator to the first instruction of the bytecode. */
  inline const_iterator begin() const {
    return this->code.begin();
  }

  /** Returns an iterator pointing right after the last instruction. */
  inline const_iterator end() const {
    return this->code.end();
  }
};


}
}
}

#endif
