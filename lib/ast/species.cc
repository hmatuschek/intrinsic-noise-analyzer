#include "species.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Ast;


Species::Species(const std::string &id, Compartment *compartment, bool is_const)
  : VariableDefinition(Node::SPECIES_DEFINITION, id, is_const),
    compartment(compartment)
{
  // Pass...
}


Species::Species(const std::string &id, const GiNaC::ex &init_val,
                 Compartment *compartment, const std::string &name, bool is_const)
  : VariableDefinition(Node::SPECIES_DEFINITION, id, init_val, name, is_const),
    compartment(compartment)
{
  // pass...
}


Compartment * Species::getCompartment() { return this->compartment; }
const Compartment * Species::getCompartment() const { return this->compartment; }

void Species::setCompartment(Compartment *compartment) { this->compartment = compartment; }


void
Species::accept(Ast::Visitor &visitor) const
{
  if (Species::Visitor *var_vis = dynamic_cast<Species::Visitor *>(&visitor)) {
    var_vis->visit(this);
  } else {
    VariableDefinition::accept(visitor);
  }
}


void
Species::apply(Ast::Operator &op)
{
  if (Species::Operator *var_op = dynamic_cast<Species::Operator *>(&op)) {
    var_op->act(this);
  } else {
    VariableDefinition::apply(op);
  }
}
