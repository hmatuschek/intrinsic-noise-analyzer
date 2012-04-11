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


Definition::~Definition()
{
  // Done.
}


const std::string &
Definition::getIdentifier() const
{
  return this->identifier;
}


bool
Definition::hasName() const
{
  return 0 != this->name.size();
}


const std::string &
Definition::getName() const
{
  return this->name;
}


void
Definition::setName(const std::string &name)
{
  this->name = name;
}
