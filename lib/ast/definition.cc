#include "definition.hh"

using namespace Fluc::Ast;


Definition::Definition(const std::string &id, Node::NodeType node_type)
  : Node(node_type), identifier(id), name()
{
  // Done.
}


Definition::Definition(const std::string &id, const std::string &name, Node::NodeType node_type)
  : Node(node_type), identifier(id), name(name)
{
  // Done.
}


Definition::~Definition() {
  // Done.
}


const std::string &
Definition::getIdentifier() const {
  return this->identifier;
}


bool
Definition::hasName() const {
  return 0 != this->name.size();
}


const std::string &
Definition::getName() const {
  return this->name;
}


void
Definition::setName(const std::string &name) {
  this->name = name;
}


void
Definition::accept(Ast::Visitor &visitor) const
{
  if (Definition::Visitor *def_vis = dynamic_cast<Definition::Visitor *>(&visitor)) {
    def_vis->visit(this);
  } else {
    Node::accept(visitor);
  }
}


void
Definition::apply(Ast::Operator &op)
{
  if (Definition::Operator *def_op = dynamic_cast<Definition::Operator *>(&op)) {
    def_op->act(this);
  } else {
    Node::apply(op);
  }
}
