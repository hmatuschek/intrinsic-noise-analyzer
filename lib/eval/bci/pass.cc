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
  // Can only be applied on ADD, MUL and POW where RHS is immediate Constant:
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

  if (Instruction::POW == value->opCode()) {
      if (value->immediateValue() == 1.0) {
          SmartPtr<Value> arg0 = value->argument(0);
          value = arg0;
          return true;
      }
      if (value->immediateValue() == 0.0) {
          value = Value::createPush(1.0);
          return true;
      }
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
 * Implementation of IPowPass
 * ********************************************************************************************* */
bool
IPowPass::handleValue(SmartPtr<Value> &value)
{
    if (Instruction::POW != value->opCode())
        return false;
    if (! value->hasImmediateValue())
        return false;

    // check if exponent is integer and 0 < exponent <= 5
    std::complex<double> exponent = value->immediateValue();
    if (exponent.real() != (unsigned)exponent.real()|| 0 != exponent.imag() ||
            0 >= exponent.real() || exponent.real() > 8)
        return false;

    SmartPtr<Value> arg0 = value->argument(0);
    value = Value::createIPow(arg0, (unsigned)exponent.real());

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


/* ********************************************************************************************* *
 * Implementation of ConstantPropagation
 * ********************************************************************************************* */
bool
ConstantPropagation::handleValue(SmartPtr<Value> &value)
{
  if (Instruction::MUL != value->opCode() || value->hasImmediateValue())
    return false;

  SmartPtr<Value> lhs = value->argument(0);
  SmartPtr<Value> rhs = value->argument(1);

  // Check if form is ((x * c) * rhs)
  if (Instruction::MUL == lhs->opCode() && lhs->hasImmediateValue()) {
    SmartPtr<Value> x = lhs->argument(0);
    std::complex<double> c = lhs->immediateValue();

    // Replace value with ((x * rhs) * c)
    lhs = Value::createMul(x, rhs);
    value = Value::createMul(lhs, c);
    return true;
  }

  // or if form is (lhs * (x * c))
  if (Instruction::MUL == rhs->opCode() && rhs->hasImmediateValue()) {
    SmartPtr<Value> x = rhs->argument(0);
    std::complex<double> c = rhs->immediateValue();

    // Replace value with ((lhs * x) * c)
    lhs = Value::createMul(lhs, x);
    value = Value::createMul(lhs, c);
    return true;
  }

  return false;
}


/* ********************************************************************************************* *
 * Implementation of InstructionCanonization
 * ********************************************************************************************* */
bool
InstructionCanonization::handleValue(SmartPtr<Value> &value)
{
  if (Instruction::ADD != value->opCode() || value->hasImmediateValue())
    return false;

  SmartPtr<Value> lhs = value->argument(0);
  SmartPtr<Value> rhs = value->argument(1);
  bool lhs_is_neg = (Instruction::MUL == lhs->opCode()) && lhs->hasImmediateValue()
      && (-1 == lhs->immediateValue().real()) && (0 == lhs->immediateValue().imag());
  bool rhs_is_neg = (Instruction::MUL == rhs->opCode()) && rhs->hasImmediateValue()
      && (-1 == rhs->immediateValue().real()) && (0 == rhs->immediateValue().imag());

  // Shortcut:
  if ( (! lhs_is_neg) && (! rhs_is_neg) )
    return false;

  // (x * -1) + rhs -> rhs - x
  if ( lhs_is_neg && (! rhs_is_neg)) {
    SmartPtr<Value> x = lhs->argument(0);
    value = Value::createSub(rhs, x);
    return true;
  }

  // lhs + (x * -1) -> lhs - x:
  if ( (! lhs_is_neg) && rhs_is_neg) {
    SmartPtr<Value> x = rhs->argument(0);
    value = Value::createSub(lhs, x);
    return true;
  }

  // ((x * -1) + (y * -1)) -> ((x + y) * -1)
  if (lhs_is_neg && rhs_is_neg) {
    SmartPtr<Value> x = lhs->argument(0);
    SmartPtr<Value> y = rhs->argument(0);
    value = Value::createMul(Value::createAdd(x, y), -1.0);
    return true;
  }

  return false;
}
