#include "definition.hh"

using namespace iNA::Ast;


Definition::Definition(const std::string &id, Node::NodeType node_type)
  : Node(node_type), _identifier(id), _name()
{
  // Done.
}


Definition::Definition(const std::string &id, const std::string &name, Node::NodeType node_type)
  : Node(node_type), _identifier(id), _name(name)
{
  // Done.
}


Definition::~Definition() {
  // Done.
}


const std::string &
Definition::getIdentifier() const {
  return _identifier;
}

void
Definition::setIdentifier(const std::string &id) {
  _identifier = id;
}

bool
Definition::hasName() const {
  return 0 != this->_name.size();
}


const std::string &
Definition::getName() const {
  return this->_name;
}


void
Definition::setName(const std::string &name) {
  this->_name = name;
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
