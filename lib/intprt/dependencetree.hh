#ifndef __FLUC_INTPRT_DEPENDENCETREE_HH__
#define __FLUC_INTPRT_DEPENDENCETREE_HH__

#include <vector>
#include "instruction.hh"
#include "smartptr.hh"


namespace Fluc {
namespace Intprt {

class Value;

/**
 * Represents a single node in the dependence graph of values.
 *
 * @ingroup intprt
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
   * @todo Remove?
   */
  virtual ~Node();

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
   *
   * @deprecated This method shall be removed.
   */
  virtual void serialize(std::vector<Instruction> &code);

  /**
   * Recusively serializes the node into the given byte-code vector.
   */
  virtual void serialize(ByteCode &code);
};


/**
 * Represents a value in the dependence graph.
 *
 * @ingroup intprt
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
  double immediateValue() const;

  /**
   * Returns the immediate value of the instruction as an integer index.
   */
  size_t immediateIndex() const;

  /**
   * Recursively serializes the instruction and all values it depends on into the given
   * bytecode vector.
   *
   * @deprecated Remove this method.
   */
  void serialize(std::vector<Instruction> &code);

  /**
   * Recursively serializes the instruction and all values it depends on into the given
   * bytecode vector.
   */
  void serialize(ByteCode &code);


public:
  /**
   * Constructs an ADD instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createAdd(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs an ADD instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createAdd(SmartPtr<Value> lhs, double rhs);

  /**
   * Constructs a SUB instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createSub(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a SUB instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createSub(SmartPtr<Value> lhs, double rhs);

  /**
   * Constructs a MUL instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createMul(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a MUL instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createMul(SmartPtr<Value> lhs, double rhs);

  /**
   * Constructs a DIV instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createDiv(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a DIV instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createDiv(SmartPtr<Value> lhs, double rhs);

  /**
   * Constructs a POW instruction dependeing on the two given values.
   */
  static SmartPtr<Value> createPow(SmartPtr<Value> lhs, SmartPtr<Value> rhs);

  /**
   * Constructs a POW instruction depending on the given LHS value and the
   * immediate RHS value.
   */
  static SmartPtr<Value> createPow(SmartPtr<Value> lhs, double rhs);

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
   * Creates a PUSH instruction, pushing the given immediate value on the stack.
   */
  static SmartPtr<Value> createPush(double value);

  /**
   * Creates a dependence graph node representing the given instruction and depending on the
   * vector of arguments.
   */
  static SmartPtr<Value> create(const Instruction &instruction,
                                const std::vector< SmartPtr<Value> > &arguments);
};


/**
 * Represents the root of the dependence graph.
 */
class DependenceTree : public Node
{
public:
  /**
   * Constructs a dependence-tree from the given byte-code vector.
   *
   * @deprecated Remove this constructor.
   */
  DependenceTree(const std::vector<Instruction> &code);


  /**
   * Constructs a dependence-tree from the given byte-code.
   */
  DependenceTree(const ByteCode &code);
};


}
}

#endif
