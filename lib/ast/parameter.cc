#include "parameter.hh"

using namespace iNA;
using namespace iNA::Ast;


Parameter::Parameter(const std::string &id, const Unit &unit, bool is_const)
  : VariableDefinition(Node::PARAMETER_DEFINITION, id, is_const), _unit(unit)
{
  // Pass...
}


Parameter::Parameter(const std::string &id, const GiNaC::ex &init_val, const Unit &unit, bool is_const)
  : VariableDefinition(Node::PARAMETER_DEFINITION, id, init_val, "", is_const), _unit(unit)
{
  // Pass...
}


const Unit &
Parameter::getUnit() const {
  return _unit;
}

void
Parameter::setUnit(const Unit &unit) {
  _unit = unit;
}


void
Parameter::accept(Ast::Visitor &visitor) const
{
  if (Parameter::Visitor *param_vis = dynamic_cast<Parameter::Visitor *>(&visitor)) {
    param_vis->visit(this);
  } else {
    VariableDefinition::accept(visitor);
  }
}


void
Parameter::apply(Ast::Operator &op)
{
  if (Parameter::Operator *param_op = dynamic_cast<Parameter::Operator *>(&op)) {
    param_op->act(this);
  } else {
    VariableDefinition::apply(op);
  }
}
