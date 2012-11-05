#include "node.hh"
#include "visitor.hh"

using namespace iNA::Ast;


/* ********************************************************************************************* *
 * Implementation of Ast::Node:
 * ********************************************************************************************* */
Node::Node(Node::NodeType node_type)
  :  node_type(node_type)
{
  // Done.
}

Node::~Node()
{
  // Done.
}


void
Node::accept(Ast::Visitor &visitor) const {
  // If visitor implements a visitor for Ast::Node instances:
  if (Ast::Node::Visitor *node_vis = dynamic_cast<Ast::Node::Visitor *>(&visitor)) {
    node_vis->visit(this);
  }
}

void
Node::apply(Ast::Operator &op) {
  // If operator implements an operator for Ast::Node instances:
  if (Ast::Node::Operator *node_op = dynamic_cast<Ast::Node::Operator *>(&op)) {
    node_op->act(this);
  }
}

void
Node::traverse(Ast::Visitor &visitor) const {
  // pass...
}

void
Node::traverse(Ast::Operator &op) {
  // pass...
}

Node::NodeType
Node::getNodeType() const
{
  return this->node_type;
}


bool
Node::isConstraint(const Node *node)
{
  switch (node->getNodeType())
  {
  case Node::ALGEBRAIC_CONSTRAINT:
    return true;

  default:
    return false;
  }
}


bool
Node::isAlgebraicConstraint(const Node *node)
{
  return Node::ALGEBRAIC_CONSTRAINT == node->getNodeType();
}


bool
Node::isDefinition(const Node *node)
{
  switch(node->getNodeType())
  {
  case FUNCTION_DEFINITION:
  case FUNCTION_ARGUMENT:
  case SPECIES_DEFINITION:
  case PARAMETER_DEFINITION:
  case COMPARTMENT_DEFINITION:
  case REACTION_DEFINITION:
    return true;

  default:
    return false;
  }
}


bool
Node::isFunctionDefinition(const Node *node)
{
  return Node::FUNCTION_DEFINITION == node->getNodeType();
}


bool
Node::isVariableDefinition(const Node *node)
{
  switch (node->getNodeType())
  {
  case Node::FUNCTION_ARGUMENT:
  case Node::SPECIES_DEFINITION:
  case Node::PARAMETER_DEFINITION:
  case Node::COMPARTMENT_DEFINITION:
    return true;

  default:
    break;
  }

  return false;
}


bool
Node::isSpecies(const Node *node)
{
  return Node::SPECIES_DEFINITION == node->getNodeType();
}


bool
Node::isParameter(const Node *node)
{
  return Node::PARAMETER_DEFINITION == node->getNodeType();
}


bool
Node::isCompartment(const Node *node)
{
  return Node::COMPARTMENT_DEFINITION == node->getNodeType();
}


bool
Node::isUnitDefinition(const Node *node)
{
  return Node::UNIT_DEFINITION == node->getNodeType();
}


bool
Node::isRule(const Node *node)
{
  switch (node->getNodeType())
  {
  case Node::ASSIGNMENT_RULE:
  case Node::RATE_RULE:
    return true;

  default:
    return false;
  }
}


bool
Node::isAssignmentRule(const Node *node)
{
  return Node::ASSIGNMENT_RULE == node->getNodeType();
}


bool
Node::isRateRule(const Node *node)
{
  return Node::RATE_RULE == node->getNodeType();
}



bool
Node::isReactionDefinition(const Node *node)
{
  return Node::REACTION_DEFINITION == node->getNodeType();
}


bool
Node::isKineticLaw(const Node *node)
{
  return Node::KINETIC_LAW == node->getNodeType();
}
