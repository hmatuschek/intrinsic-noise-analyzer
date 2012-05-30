#ifndef __FLUC_EVALUATE_BCI_DEPENDENCETREE_HH__
#define __FLUC_EVALUATE_BCI_DEPENDENCETREE_HH__

#include <vector>
#include "code.hh"
#include "smartptr.hh"


namespace Fluc {
namespace Eval {
namespace bci {

class Value;

/**
 * Represents a single node in the dependence graph of values.
 *
 * The dependence graph is a intermediate representation used by the optimisation passes to
 * perfrom transformations on the code.
 *
 * @ingroup bci
 */
class Node
{
public:
  /**
   * Defines the iterator over values this node depends on.
   */
  typedef std::vector< SmartPtr<Value> >::iterator iterator;


protected:
  /**
   * Holds a list of all values, this node depends on.
   */
  std::vector< SmartPtr<Value> > arguments;


protected:
  /**
   * Constructs a new node.
   */
  Node(const std::vector< SmartPtr<Value> > arguments);


public:
  /**
   * Returns the number of values, this node depends on.
   */
  size_t numArguments() const;

  /**
   * Returns the i-th value.
   */
  SmartPtr<Value> &argument(size_t idx);

  /**
   * Replaces the i-th value.
   */
  void setArgument(SmartPtr<Value>, size_t idx);

  /**
   * Points to the first value, this node depends on.
   */
  std::vector< SmartPtr<Value> >::iterator begin();

  /**
   * Points right after the last value, this node depends on.
   */
  std::vector< SmartPtr<Value> >::iterator end();

  /**
   * Recusively serializes the node into the given byte-code vector.
   */
  virtual void serialize(Code &code);
};


/**
 * Represents a value in the dependence graph.
 *
 * @ingroup bci
 */
class Value : public Node
{
protected:
  /**
   * Represents an instruction, generating a value.
   */
  Instruction instruction;


protected:
  /**
   * Hidden constructor, avoids direct instantiation.
   */
  Value(const Instruction &instruction, const std::vector< SmartPtr<Value> > &arguments);


public:
  /**
   * Retunrs the opcode of the associated instruction.
   */
  Instruction::OpCode opCode() const;

  /**
   * Returns true, if the instruction has a immediate value.
   */
  bool hasImmediateValue() const;

  /**
   * Returns the immediate value of the instruction as a double value.
   */
  std::complex<double> immediateValue() const;

  /**
   * Returns the immediate value of the instruction as an integer index.
   */
  size_t immediateIndex() const;

  /**
   * Recursively serializes the instruction and all values it depends on into the given
   * bytecode vector.
   */
  void serialize(Code &code);


public:
  /**
   * Constructs an ADD instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createAdd(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs an ADD instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createAdd(SmartPtr<Value> lhs, std::complex<double> rhs);

  /**
   * Constructs a SUB instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createSub(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a SUB instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createSub(SmartPtr<Value> lhs, std::complex<double> rhs);

  /**
   * Constructs a MUL instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createMul(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a MUL instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createMul(SmartPtr<Value> lhs, std::complex<double> rhs);

  /**
   * Constructs a DIV instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createDiv(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a DIV instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createDiv(SmartPtr<Value> lhs, std::complex<double> rhs);

  /**
   * Constructs a POW instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createPow(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a POW instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createPow(SmartPtr<Value> lhs, std::complex<double> rhs);

  /**
   * Constructs a IPOW instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createIPow(SmartPtr<Value> lhs, size_t rhs);

  /**
   * Constructs a LOAD instruction, pushing a value from the input vector at index @c index
   * on the internal stack.
   */
  static SmartPtr<Value> createLoad(size_t index);

  /**
   * Constructs an Store instruction dependeing on the given value and stores that value
   * into the output-vector at the given index.
   */
  static SmartPtr<Value> createStore(SmartPtr<Value> value, size_t index);

  /**
   * Constructs a STORE_ZERO instruction, that stores an 0 into the output vector at the given
   * index.
   */
  static SmartPtr<Value> createStoreZero(size_t index);

  /**
   * Creates a PUSH instruction, pushing the given immediate value on the stack.
   */
  static SmartPtr<Value> createPush(std::complex<double> value);

  /**
   * Creates a CALL instruction.
   */
  static SmartPtr<Value> createCall(Instruction::FunctionCode code, SmartPtr<Value> arg);

  /**
   * Creates a dependence graph node representing the given instruction and depending on the
   * vector of arguments.
   */
  static SmartPtr<Value> create(const Instruction &instruction,
                                const std::vector< SmartPtr<Value> > &arguments);
};


/**
 * Represents the root of the dependence graph.
 *
 * @ingroup bci
 */
class DependenceTree : public Node
{
public:
  /**
   * Constructs a dependence-tree from the given byte-code.
   */
  DependenceTree(const Code &code);
};


}
}
}

#endif
