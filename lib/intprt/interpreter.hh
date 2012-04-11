#ifndef __FLUC_INTPRT_INTERPRETER_HH__
#define __FLUC_INTPRT_INTERPRETER_HH__

#include <ginac/ginac.h>
#include "ginacsupportforeigen.hh"

#include <vector>
#include <list>
#include <map>
#include <cmath>

#include "ast/model.hh"
#include "instruction.hh"


namespace Fluc {
namespace Intprt {


/**
 * The byte-code interpreter.
 *
 * This class implements a simple stack-machine, that interpretes some byte-code efficiently.
 *
 * @deprecated Use one of the interpreters in @c Fluc::Evaluate or the default interpreter in
 *             @c Fluc::Eval.
 *
 * @ingroup intprt
 */
template <class InType, class OutType=InType>
class Interpreter
{
public:
  /**
   * Defines a value type as a union of double and complex-double values.
   *
   * The type of the value is determined by the instruction that processes this
   * value on the stack.
   */
  class Value {
  private:
    std::complex<double> _value;

  public:
    Value() {}
    Value(const double &value): _value(value) { }
    Value(const std::complex<double> &value): _value(value) { }

    inline double &asValue() { return _value.real(); }
    inline const double &asValue() const { return _value.real(); }
    inline std::complex<double> &asComplex() { return _value; }
    inline const std::complex<double> &asComplex() const { return _value; }
  };


protected:
  /**
   * Holds the interpreter-stack.
   *
   * @todo Use Interpreter::Value instead.
   */
  std::vector<Value> stack;

  /**
   * Holds a weak reference to the code to be evaluated.
   */
  ByteCode *code;


public:
  /**
   * Constructs an interpreter with-out any byte-code, you may add some code to be executed
   * using @c setCode.
   */
  Interpreter()
    : stack(0), code(0)
  {
    // Pass...
  }

  /**
   * Constructs an interpreter with the given byte-code.
   */
  Interpreter(ByteCode *code)
    : stack(code->getMinStackSize()), code(code)
  {
    // pass...
  }

  /**
   * Resets the code.
   */
  void setCode(ByteCode *code)
  {
    this->code = code;
    this->stack.reserve(code->getMinStackSize());
  }

  /**
   * Executes the byte-code in a stack-machine.
   */
  inline void run(const InType &input, OutType &output)
  {
    double rhs;

    // Loop through code:
    for (std::vector<Instruction>::iterator inst=this->code->begin(); inst!=this->code->end(); inst++)
    {
      // Dispatch by instruction OpCode:
      switch (inst->opcode)
      {
      case Instruction::ADD:
        if (inst->valueImmediate) {
          rhs = inst->value.asValue;
        } else {
          rhs = this->stack.back().asValue(); this->stack.pop_back();
        }
        this->stack.back().asValue() += rhs;
        break;

      case Instruction::SUB:
        if (inst->valueImmediate) {
          rhs = inst->value.asValue;
        } else {
          rhs = this->stack.back().asValue(); this->stack.pop_back();
        }
        this->stack.back().asValue() -= rhs;
        break;

      case Instruction::MUL:
        if (inst->valueImmediate) {
          rhs = inst->value.asValue;
        } else {
          rhs = this->stack.back().asValue(); this->stack.pop_back();
        }
        this->stack.back().asValue() *= rhs;
        break;

      case Instruction::DIV:
        if (inst->valueImmediate) {
          rhs = inst->value.asValue;
        } else {
          rhs = this->stack.back().asValue(); this->stack.pop_back();
        }
        this->stack.back().asValue() /= rhs;
        break;

      case Instruction::POW:
        if (inst->valueImmediate) {
          rhs = inst->value.asValue;
        } else {
          rhs = this->stack.back().asValue(); this->stack.pop_back();
        }
        this->stack.back().asValue() = std::pow(this->stack.back().asValue(), rhs);
        break;

      case Instruction::LOAD:
        this->stack.push_back(Value(input(inst->value.asIndex)));
        break;

      case Instruction::STORE:
        output(inst->value.asIndex) = this->stack.back().asValue();
        this->stack.pop_back();
        break;

      case Instruction::PUSH:
        this->stack.push_back(Value(inst->value.asValue));
        break;
      }
    }
  }


  /**
   * Returns the current stack-size.
   */
  size_t getStackSize() const
  {
    return this->stack.size();
  }


  /**
   * Removes the last value on the stack and returns it.
   */
  double popValue()
  {
    double value = this->stack.back().asValue();
    this->stack.pop_back();
    return value;
  }
};


}
}


#endif
