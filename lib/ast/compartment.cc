#include "compartment.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Ast;


Compartment::Compartment(const std::string &id, SpatialDimension dim, bool is_const)
  : VariableDefinition(Node::COMPARTMENT_DEFINITION, id, is_const), dimension(dim)
{
  // Pass...
}

Compartment::Compartment(const std::string &id, const GiNaC::ex &init_val,
                         SpatialDimension dim, bool is_const)
  : VariableDefinition(Node::COMPARTMENT_DEFINITION, id, init_val, "", is_const),
    dimension(dim)
{
  // Pass...
}

Compartment::SpatialDimension
Compartment::getDimension() const
{
  return this->dimension;
}


void
Compartment::accept(Ast::Visitor &visitor) const
{
  if (Compartment::Visitor *comp_vis = dynamic_cast<Compartment::Visitor *>(&visitor)) {
    comp_vis->visit(this);
  } else {
    VariableDefinition::accept(visitor);
  }
}


void
Compartment::apply(Ast::Operator &op)
{
  if (Compartment::Operator *comp_op = dynamic_cast<Compartment::Operator *>(&op)) {
    comp_op->act(this);
  } else {
    VariableDefinition::apply(op);
  }
}
