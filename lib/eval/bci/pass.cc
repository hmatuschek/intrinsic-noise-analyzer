#include "pass.hh"

using namespace Fluc;
using namespace Fluc::Evaluate::bci;



void
PassManager::addPass(Pass *pass)
{
  this->_passes.push_back(pass);
}


void
PassManager::apply(Code &code)
{
  DependenceTree module(code);

  for (size_t i=0; i<module.numArguments(); i++)
  {
    // Apply rules on expression until they do not apply anymore
    this->handleValue(module.argument(i));
  }

  // Serialize optimized dependence-tree into byte-code:
  Code new_code; module.serialize(new_code);
  // Override "old" byte-code.
  code = new_code;
}



bool
PassManager::handleValue(SmartPtr<Value> &value)
{
  // First, apply passes
  bool matched = this->applyOnValue(value);

  // then, traverse into child-nodes:
  bool child_matched = false;
  for (size_t i=0; i<value->numArguments(); i++)
  {
    child_matched = this->handleValue(value->argument(i));
  }

  // If one of the child nodes was modified, re-run passes on this node:
  if (child_matched)
    matched |= this->applyOnValue(value);

  return matched | child_matched;
}


bool
PassManager::applyOnValue(SmartPtr<Value> &value)
{
  bool matched = false;

rerun:
  for (std::list<Pass *>::iterator pass = this->_passes.begin(); pass != this->_passes.end(); pass++)
  {
    if ((*pass)->handleValue(value)) {
      matched = true; goto rerun;
    }
  }

  return matched;
}




/* ********************************************************************************************* *
 * Implementation of ImmediateValueRHSPass
 * ********************************************************************************************* */
bool
ImmediateValueRHSPass::handleValue(SmartPtr<Value> &value)
{
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


/* ********************************************************************************************* *
 * Implementation of RemoveUnitsPass
 * ********************************************************************************************* */
bool
RemoveUnitsPass::handleValue(SmartPtr<Value> &value)
{
  // Can only be applied on ADD and MUL where RHS is immediate Constant:
  if (! value->hasImmediateValue())
    return false;

  if (Instruction::ADD == value->opCode() && value->immediateValue() == 0.0)
  {
    // Simply replace instruction with LHS value:
    SmartPtr<Value> arg0 = value->argument(0);
    value = arg0;
    return true;
  }

  if (Instruction::MUL == value->opCode() && value->immediateValue() == 1.0)
  {
    SmartPtr<Value> arg0 = value->argument(0);
    value = arg0;
    return true;
  }

  // nop, did not fit:
  return false;
}



/* ********************************************************************************************* *
 * Implementation of ConstantFoldingPass
 * ********************************************************************************************* */
bool
ConstantFoldingPass::handleValue(SmartPtr<Value> &value)
{
  // If not a function call -> false:
  if (Instruction::CALL != value->opCode()) {
    return false;
  }

  // If argument is not a constant -> false:
  if (Instruction::PUSH != value->argument(0)->opCode()) {
    return false;
  }

  // Otherwise replace function call with evaluation:
  std::complex<double> arg = value->argument(0)->immediateValue();
  switch (Instruction::FunctionCode(value->immediateIndex())) {
  case Instruction::FUNCTION_ABS:
    value = Value::createPush(std::abs(arg));
    break;
  case Instruction::FUNCTION_LOG:
    value = Value::createPush(std::log(arg));
    break;
  case Instruction::FUNCTION_EXP:
    value = Value::createPush(std::exp(arg));
    break;
  }

  return true;
}


/* ********************************************************************************************* *
 * Implementation of ZeroStorePass
 * ********************************************************************************************* */
bool
ZeroStorePass::handleValue(SmartPtr<Value> &value)
{
  if (Instruction::STORE != value->opCode())
    return false;

  if (Instruction::PUSH != value->argument(0)->opCode())
    return false;

  std::complex<double> arg = value->argument(0)->immediateValue();
  if (0 != arg.real() || 0 != arg.imag())
    return false;

  value = Value::createStoreZero(value->immediateIndex());

  return true;
}
