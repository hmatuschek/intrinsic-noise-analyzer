#include "dependencetree.hh"
#include <list>


using namespace Fluc;
using namespace Fluc::Intprt;



/* ********************************************************************************************* *
 * Implementation of Node, base class for dependence graph:
 * ********************************************************************************************* */
Node::Node(const std::vector< SmartPtr<Value> > arguments)
  : arguments(arguments)
{
  // Pass...
}


Node::~Node()
{
}


size_t
Node::numArguments() const
{
  return this->arguments.size();
}


SmartPtr<Value> &
Node::argument(size_t idx)
{
  return this->arguments[idx];
}


void
Node::setArgument(SmartPtr<Value> value, size_t idx)
{
  this->arguments[idx] = value;
}


std::vector< SmartPtr<Value> >::iterator
Node::begin()
{
  return this->arguments.begin();
}

std::vector< SmartPtr<Value> >::iterator
Node::end()
{
  return this->arguments.end();
}


void
Node::serialize(std::vector<Instruction> &code)
{
  for(size_t i=0; i<this->numArguments(); i++)
  {
    this->argument(i)->serialize(code);
  }
}


void
Node::serialize(ByteCode &code)
{
  for(size_t i=0; i<this->numArguments(); i++)
  {
    this->argument(i)->serialize(code);
  }
}


/* ********************************************************************************************* *
 * Implementation of Value
 * ********************************************************************************************* */
Value::Value(const Instruction &instruction, const std::vector< SmartPtr<Value> > &arguments)
  : Node(arguments), instruction(instruction)
{
  // Pass...
}


Instruction::OpCode
Value::opCode() const
{
  return this->instruction.opcode;
}


bool
Value::hasImmediateValue() const
{
  return this->instruction.valueImmediate;
}


double
Value::immediateValue() const
{
  return this->instruction.value.asValue;
}


size_t
Value::immediateIndex() const
{
  return this->instruction.value.asIndex;
}


void
Value::serialize(std::vector<Instruction> &code)
{
  // First, serialize arguments:
  Node::serialize(code);

  // Now this node:
  code.push_back(this->instruction);
}


void
Value::serialize(ByteCode &code)
{
  // First, serialize arguments:
  Node::serialize(code);

  // Now this node:
  code << this->instruction;
}


SmartPtr<Value>
Value::createAdd(SmartPtr<Value> lhs, double rhs)
{
  std::vector< SmartPtr<Value> > args(1); args[0] = lhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::ADD, rhs), args));
}


SmartPtr<Value>
Value::createAdd(SmartPtr<Value> lhs, SmartPtr<Value> rhs)
{
  std::vector< SmartPtr<Value> > args(2); args[0] = lhs; args[1] = rhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::ADD), args));
}


SmartPtr<Value>
Value::createSub(SmartPtr<Value> lhs, double rhs)
{
  std::vector< SmartPtr<Value> > args(1); args[0] = lhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::SUB, rhs), args));
}


SmartPtr<Value>
Value::createSub(SmartPtr<Value> lhs, SmartPtr<Value> rhs)
{
  std::vector< SmartPtr<Value> > args(2); args[0] = lhs; args[1] = rhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::SUB), args));
}


SmartPtr<Value>
Value::createMul(SmartPtr<Value> lhs, double rhs)
{
  std::vector< SmartPtr<Value> > args(1); args[0] = lhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::MUL, rhs), args));
}


SmartPtr<Value>
Value::createMul(SmartPtr<Value> lhs, SmartPtr<Value> rhs)
{
  std::vector< SmartPtr<Value> > args(2); args[0] = lhs; args[1] = rhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::MUL), args));
}


SmartPtr<Value>
Value::createDiv(SmartPtr<Value> lhs, double rhs)
{
  std::vector< SmartPtr<Value> > args(1); args[0] = lhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::DIV, rhs), args));
}


SmartPtr<Value>
Value::createDiv(SmartPtr<Value> lhs, SmartPtr<Value> rhs)
{
  std::vector< SmartPtr<Value> > args(2); args[0] = lhs; args[1] = rhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::DIV), args));
}


SmartPtr<Value>
Value::createPow(SmartPtr<Value> lhs, double rhs)
{
  std::vector< SmartPtr<Value> > args(1); args[0] = lhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::POW, rhs), args));
}


SmartPtr<Value>
Value::createPow(SmartPtr<Value> lhs, SmartPtr<Value> rhs)
{
  std::vector< SmartPtr<Value> > args(2); args[0] = lhs; args[1] = rhs;
  return SmartPtr<Value>(new Value(Instruction(Instruction::POW), args));
}


SmartPtr<Value>
Value::createLoad(size_t index)
{
  std::vector< SmartPtr<Value> > args;
  return SmartPtr<Value>(new Value(Instruction(Instruction::LOAD, index), args));
}


SmartPtr<Value>
Value::createStore(SmartPtr<Value> value, size_t index)
{
  std::vector< SmartPtr<Value> > args(1); args[0] = value;
  return SmartPtr<Value>(new Value(Instruction(Instruction::STORE, index), args));
}


SmartPtr<Value>
Value::createPush(double value)
{
  std::vector< SmartPtr<Value> > args;
  return SmartPtr<Value>(new Value(Instruction(Instruction::PUSH, value), args));
}


SmartPtr<Value>
Value::create(const Instruction &instruction, const std::vector<SmartPtr<Value> > &arguments)
{
  return SmartPtr<Value>(new Value(instruction, arguments));
}



/* ********************************************************************************************* *
 * Implementation of Module
 * ********************************************************************************************* */
DependenceTree::DependenceTree(const std::vector<Instruction> &code)
  : Node(std::vector< SmartPtr<Value> >())
{
  std::list< SmartPtr<Value> > stack; SmartPtr<Value> lhs, rhs;

  for (size_t i=0; i<code.size(); i++)
  {
    switch (code[i].opcode)
    {
    case Instruction::ADD:
      rhs = stack.back(); stack.pop_back();
      if (code[i].valueImmediate) {
        stack.push_back(Value::createAdd(rhs, code[i].value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createAdd(lhs, rhs));
      }
      break;

    case Instruction::SUB:
      rhs = stack.back(); stack.pop_back();
      if (code[i].valueImmediate) {
        stack.push_back(Value::createSub(rhs, code[i].value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createSub(lhs, rhs));
      }
      break;

    case Instruction::MUL:
      rhs = stack.back(); stack.pop_back();
      if (code[i].valueImmediate) {
        stack.push_back(Value::createMul(rhs, code[i].value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createMul(lhs, rhs));
      }
      break;

    case Instruction::DIV:
      rhs = stack.back(); stack.pop_back();
      if (code[i].valueImmediate) {
        stack.push_back(Value::createDiv(rhs, code[i].value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createDiv(lhs, rhs));
      }
      break;

    case Instruction::POW:
      rhs = stack.back(); stack.pop_back();
      if (code[i].valueImmediate) {
        stack.push_back(Value::createPow(rhs, code[i].value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createPow(lhs, rhs));
      }
      break;

    case Instruction::LOAD:
      stack.push_back(Value::createLoad(code[i].value.asIndex));
      break;

    case Instruction::STORE:
      rhs = stack.back(); stack.pop_back();
      stack.push_back(Value::createStore(rhs, code[i].value.asIndex));
      break;

    case Instruction::PUSH:
      stack.push_back(Value::createPush(code[i].value.asValue));
      break;
    }
  }

  // Construct arguments:
  this->arguments.reserve(stack.size());
  while (stack.size() > 0) {
    lhs = stack.back(); stack.pop_back();
    this->arguments.push_back(lhs);
  }
}



DependenceTree::DependenceTree(const ByteCode &code)
  : Node(std::vector< SmartPtr<Value> >())
{
  std::list< SmartPtr<Value> > stack; SmartPtr<Value> lhs, rhs;

  for (ByteCode::const_iterator item=code.begin(); item!=code.end(); item++)
  {
    switch (item->opcode)
    {
    case Instruction::ADD:
      rhs = stack.back(); stack.pop_back();
      if (item->valueImmediate) {
        stack.push_back(Value::createAdd(rhs, item->value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createAdd(lhs, rhs));
      }
      break;

    case Instruction::SUB:
      rhs = stack.back(); stack.pop_back();
      if (item->valueImmediate) {
        stack.push_back(Value::createSub(rhs, item->value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createSub(lhs, rhs));
      }
      break;

    case Instruction::MUL:
      rhs = stack.back(); stack.pop_back();
      if (item->valueImmediate) {
        stack.push_back(Value::createMul(rhs, item->value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createMul(lhs, rhs));
      }
      break;

    case Instruction::DIV:
      rhs = stack.back(); stack.pop_back();
      if (item->valueImmediate) {
        stack.push_back(Value::createDiv(rhs, item->value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createDiv(lhs, rhs));
      }
      break;

    case Instruction::POW:
      rhs = stack.back(); stack.pop_back();
      if (item->valueImmediate) {
        stack.push_back(Value::createPow(rhs, item->value.asValue));
      } else {
        lhs = stack.back(); stack.pop_back();
        stack.push_back(Value::createPow(lhs, rhs));
      }
      break;

    case Instruction::LOAD:
      stack.push_back(Value::createLoad(item->value.asIndex));
      break;

    case Instruction::STORE:
      rhs = stack.back(); stack.pop_back();
      stack.push_back(Value::createStore(rhs, item->value.asIndex));
      break;

    case Instruction::PUSH:
      stack.push_back(Value::createPush(item->value.asValue));
      break;
    }
  }

  // Construct arguments:
  this->arguments.reserve(stack.size());
  while (stack.size() > 0) {
    lhs = stack.back(); stack.pop_back();
    this->arguments.push_back(lhs);
  }
}
