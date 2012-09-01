#ifndef __FLUC_EVALUATE_BCI_INTERPRETER_HH__
#define __FLUC_EVALUATE_BCI_INTERPRETER_HH__

#include <ginac/ginac.h>

#include <vector>
#include <list>
#include <map>
#include <cmath>

#include "ast/model.hh"
#include "code.hh"


namespace iNA {
namespace Eval {
namespace bci {



/**
 * Defines a value type as a union of double and complex-double values.
 *
 * The type of the value is determined by the instruction that processes this
 * value on the stack.
 *
 * @ingroup bci
 */
class InterpreterValue {
private:
  /**
   * Holds the actual value, if the value is real, it is stored in the real part.
   */
  std::complex<double> _value;

public:
  /**
   * Constructs an empty value.
   */
  InterpreterValue() {}

  /**
   * Constructs a real-valued value.
   */
  InterpreterValue(const double &value): _value(value) { }

  /**
   * Constructs a complex-valued value.
   */
  InterpreterValue(const std::complex<double> &value): _value(value) { }

  /**
   * Constructs a complex-valued value.
   */
  InterpreterValue(const double &real, const double &imag): _value(real, imag) { }

  /**
   * Returns the value as a real.
   */
  inline double &asValue() { return _value.real(); }

  /**
   * Returns the value as a real.
   */
  inline const double &asValue() const { return _value.real(); }

  /**
   * Returns the value as a complex.
   */
  inline std::complex<double> &asComplex() { return _value; }

  /**
   * Returns the value as a complex.
   */
  inline const std::complex<double> &asComplex() const { return _value; }
};



/**
 * This template defines the core "ALU" of the interpreter.
 *
 * @ingroup bci
 */
template <typename InScalar, typename OutScalar>
class InterpreterCore
{
public:
  /**
   * Prototype for the evaluation function.
   */
  static inline void eval(const InScalar *input, OutScalar *output,
                          std::vector<InterpreterValue> &stack);
};


/**
 * Specialization of InterpreterCore for real (double) values.
 *
 * @ingroup bci
 */
template <typename InScalar>
class InterpreterCore <InScalar, double>
{
public:
  static inline void eval(Instruction &inst, const InScalar *input, double *output,
                          std::vector<InterpreterValue> &stack)
  {
    double rhs;

    // Dispatch by instruction OpCode:
    switch (inst.opcode)
    {
    case Instruction::ADD:
      if (inst.valueImmediate) {
        rhs = inst.value.asComplex.real;
      } else {
        rhs = stack.back().asValue(); stack.pop_back();
      }
      stack.back().asValue() += rhs;
      break;

    case Instruction::SUB:
      if (inst.valueImmediate) {
        rhs = inst.value.asComplex.real;
      } else {
        rhs = stack.back().asValue(); stack.pop_back();
      }
      stack.back().asValue() -= rhs;
      break;

    case Instruction::MUL:
      if (inst.valueImmediate) {
        rhs = inst.value.asComplex.real;
      } else {
        rhs = stack.back().asValue(); stack.pop_back();
      }
      stack.back().asValue() *= rhs;
      break;

    case Instruction::DIV:
      if (inst.valueImmediate) {
        rhs = inst.value.asComplex.real;
      } else {
        rhs = stack.back().asValue(); stack.pop_back();
      }
      stack.back().asValue() /= rhs;
      break;

    case Instruction::POW:
      if (inst.valueImmediate) {
        rhs = inst.value.asComplex.real;
      } else {
        rhs = stack.back().asValue(); stack.pop_back();
      }
      stack.back().asValue() = std::pow(stack.back().asValue(), rhs);
      break;

    case Instruction::IPOW:
    {
      double x = stack.back().asValue();
      for (size_t i=1; i<inst.value.asIndex; i++) {
        stack.back().asValue() *= x;
      }
    }
      break;

    case Instruction::LOAD:
      stack.push_back(InterpreterValue(input[inst.value.asIndex]));
      break;

    case Instruction::STORE:
      output[inst.value.asIndex] = stack.back().asValue();
      stack.pop_back();
      break;

    case Instruction::STORE_ZERO:
      output[inst.value.asIndex] = 0.0;
      break;

    case Instruction::PUSH:
      stack.push_back(InterpreterValue(inst.value.asComplex.real));
      break;

    case Instruction::CALL:
      switch (Instruction::FunctionCode(inst.value.asIndex)) {
      case Instruction::FUNCTION_ABS:
        stack.back().asValue() = std::abs(stack.back().asValue());
        break;
      case Instruction::FUNCTION_LOG:
        stack.back().asValue() = std::log(stack.back().asValue());
        break;
      case Instruction::FUNCTION_EXP:
        stack.back().asValue() = std::exp(stack.back().asValue());
        break;
      }
    }
  }
};


/**
 * Specialization of InterpreterCore for complex (complex<double>) values
 *
 * @ingroup bci
 */
template <typename InScalar>
class InterpreterCore < InScalar, std::complex<double> >
{
public:
  static inline void eval(Instruction &inst, const InScalar *input,
                          std::complex<double> *output, std::vector<InterpreterValue> &stack)
  {
    std::complex<double> rhs;

    // Dispatch by instruction OpCode:
    switch (inst.opcode)
    {
    case Instruction::ADD:
      if (inst.valueImmediate) {
        rhs.real() = inst.value.asComplex.real;
        rhs.imag() = inst.value.asComplex.imag;
      } else {
        rhs = stack.back().asComplex(); stack.pop_back();
      }
      stack.back().asComplex() += rhs;
      break;

    case Instruction::SUB:
      if (inst.valueImmediate) {
        rhs.real() = inst.value.asComplex.real;
        rhs.imag() = inst.value.asComplex.imag;
      } else {
        rhs = stack.back().asComplex(); stack.pop_back();
      }
      stack.back().asComplex() -= rhs;
      break;

    case Instruction::MUL:
      if (inst.valueImmediate) {
        rhs.real() = inst.value.asComplex.real;
        rhs.imag() = inst.value.asComplex.imag;
      } else {
        rhs = stack.back().asComplex(); stack.pop_back();
      }
      stack.back().asComplex() *= rhs;
      break;

    case Instruction::DIV:
      if (inst.valueImmediate) {
        rhs.real() = inst.value.asComplex.real;
        rhs.imag() = inst.value.asComplex.imag;
      } else {
        rhs = stack.back().asComplex(); stack.pop_back();
      }
      stack.back().asComplex() /= rhs;
      break;

    case Instruction::POW:
      if (inst.valueImmediate) {
        rhs.real() = inst.value.asComplex.real;
        rhs.imag() = inst.value.asComplex.imag;
      } else {
        rhs = stack.back().asComplex(); stack.pop_back();
      }
      stack.back().asComplex() = std::pow(stack.back().asComplex(), rhs);
      break;

    case Instruction::IPOW:
    {
      std::complex<double> x = stack.back().asComplex();
      for (size_t i=1; i<inst.value.asIndex; i++) {
        stack.back().asComplex() *= x;
      }
    }
      break;

    case Instruction::LOAD:
      stack.push_back(InterpreterValue(input[inst.value.asIndex]));
      break;

    case Instruction::STORE:
      output[inst.value.asIndex] = stack.back().asComplex();
      stack.pop_back();
      break;

    case Instruction::STORE_ZERO:
      output[inst.value.asIndex] = std::complex<double>(0.0);
      break;

    case Instruction::PUSH:
      stack.push_back(InterpreterValue(inst.value.asComplex.real, inst.value.asComplex.imag));
      break;

    case Instruction::CALL:
      switch (Instruction::FunctionCode(inst.value.asIndex)) {
      case Instruction::FUNCTION_ABS:
        stack.back().asComplex() = std::abs(stack.back().asComplex());
        break;
      case Instruction::FUNCTION_LOG:
        stack.back().asComplex() = std::log(stack.back().asComplex());
        break;
      case Instruction::FUNCTION_EXP:
        stack.back().asComplex() = std::exp(stack.back().asComplex());
        break;
      }
    }
  }
};



/**
 * The byte-code interpreter.
 *
 * This class implements a simple stack-machine, that interpretes some byte-code efficiently.
 *
 * @ingroup bci
 */
template <class InType, class OutType=InType>
class Interpreter : public InterpreterCore<typename InType::Scalar, typename OutType::Scalar>
{
protected:
  /**
   * Holds the interpreter-stack.
   *
   * @todo Use Interpreter::Value instead.
   */
  std::vector<InterpreterValue> stack;

  /**
   * Holds a weak reference to the code to be evaluated.
   */
  Code *code;


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
  Interpreter(Code *code)
    : stack(code->getMinStackSize()), code(code)
  {
    // pass...
  }

  /**
   * Resets the code.
   */
  void setCode(Code *code)
  {
    this->code = code;
    this->stack.reserve(code->getMinStackSize());
  }

  /**
   * Executes the byte-code in a stack-machine.
   */
  inline void run(const InType &input, OutType &output)
  {
      this->run(input.data(), output.data());
  }

  /** Executes the byte-code in a stack-machine. */
  inline void run(const typename InType::Scalar *input, typename OutType::Scalar *output)
  {
      // Loop through code:
      for (std::vector<Instruction>::iterator inst=this->code->begin(); inst!=this->code->end(); inst++)
      {
        // Evaluate instruction:
        this->eval(*inst, input, output, this->stack);
      }
  }
};


}
}
}

#endif
