#include "parameter.hh"

using namespace Fluc;
using namespace Fluc::Ast;


Parameter::Parameter(const std::string &id, const Unit &unit, bool is_const)
  : VariableDefinition(Node::PARAMETER_DEFINITION, id, unit, is_const)
{
  // Pass...
}


Parameter::Parameter(const std::string &id, const GiNaC::ex &init_val, const Unit &unit, bool is_const)
  : VariableDefinition(Node::PARAMETER_DEFINITION, id, init_val, unit, "", is_const)
{
  // Pass...
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
