#include "pass.hh"

using namespace Fluc;
using namespace Fluc::Intprt;



void
Pass::apply(ByteCode &code)
{
  DependenceTree module(code);

  for (size_t i=0; i<module.numArguments(); i++)
  {
    // Apply rules on expression until they do not apply anymore
    while(this->handleValue(module.argument(i))) {
    }
  }

  // Serialize optimized dependence-tree into byte-code:
  ByteCode new_code; module.serialize(new_code);
  // Override "old" byte-code.
  code = new_code;
}



bool
Pass::handleValue(SmartPtr<Value> &value)
{
  // First, traverse into child-nodes:
  for (size_t i=0; i<value->numArguments(); i++)
  {
    // If there matched a rule down there, restart:
    if (this->handleValue(value->argument(i)))
      return true;
  }

  // If none of any rule applied to the node -> done:
  return false;
}



/* ********************************************************************************************* *
 * Implementation of ImmediateValueRHSPass
 * ********************************************************************************************* */
bool
ImmediateValueRHSPass::handleValue(SmartPtr<Value> &value)
{
  // First, process children, if successfull return true:
  if (Pass::handleValue(value))
    return true;

  // can only be applied on ADD and MUL operations:
  if ((Instruction::ADD!=value->opCode()) && (Instruction::MUL!=value->opCode()))
    return false;

  // If instruction has already an immediate value assinged -> done.
  if (value->hasImmediateValue())
    return false;

  // If LHS is push and RHS not -> swtich
  if ((Instruction::PUSH == value->argument(0)->opCode()) &&
      (Instruction::PUSH != value->argument(1)->opCode()))
  {
    // Swap LHS and RHS values:
    SmartPtr<Value> lhs = value->argument(0);
    value->setArgument(value->argument(1), 0);
    value->setArgument(lhs, 1);

    // Applied:
    return true;
  }

  return false;
}



/* ********************************************************************************************* *
 * Implementation of ImmediateValuePass
 * ********************************************************************************************* */
bool
ImmediateValuePass::handleValue(SmartPtr<Value> &value)
{
  // First, process children, if successfull return true:
  if (Pass::handleValue(value))
    return true;

  // Can only be applied on ADD, SUB, MUL, DIV, POW:
  switch (value->opCode())
  {
  case Instruction::ADD:
  case Instruction::SUB:
  case Instruction::MUL:
  case Instruction::DIV:
  case Instruction::POW:
    break;

  default:
    return false;
  }

  // If instruction has already an immediate value assinged -> done.
  if (value->hasImmediateValue())
    return false;

  // Check if RHS value is a PUSH instruction:
  if (Instruction::PUSH != value->argument(1)->opCode())
    return false;

  // Construct a new instruction replacing the current one:
  std::vector< SmartPtr<Value> > args(1); args[0] = value->argument(0);
  value = Value::create(Instruction(value->opCode(), value->argument(1)->immediateValue()), args);

  return true;
}


bool
RemoveUnitsPass::handleValue(SmartPtr<Value> &value)
{
  // First, process children, if successfull return true:
  if (Pass::handleValue(value))
    return true;

  // Can only be applied on ADD and MUL where RHS is immediate Constant:
  if (! value->hasImmediateValue())
    return false;

  if (Instruction::ADD == value->opCode() && 0 == value->immediateValue())
  {
    // Simply replace instruction with LHS value:
    SmartPtr<Value> arg0 = value->argument(0);
    value = arg0;
    return true;
  }

  if (Instruction::MUL == value->opCode() && 1 == value->immediateValue())
  {
    SmartPtr<Value> arg0 = value->argument(0);
    value = arg0;
    return true;
  }

  // nop, did not fit:
  return false;
}
