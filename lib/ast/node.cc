#include "node.hh"

using namespace Fluc::Ast;


Node::Node(Node::NodeType node_type)
  :  node_type(node_type)
{
  // Done.
}

Node::~Node()
{
  // Done.
}


Node::NodeType
Node::getNodeType()
{
  return this->node_type;
}


bool
Node::isConstraint(Node *node)
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
Node::isAlgebraicConstraint(Node *node)
{
  return Node::ALGEBRAIC_CONSTRAINT == node->getNodeType();
}


bool
Node::isDefinition(Node *node)
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
Node::isFunctionDefinition(Node *node)
{
  return Node::FUNCTION_DEFINITION == node->getNodeType();
}


bool
Node::isVariableDefinition(Node *node)
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
Node::isSpecies(Node *node)
{
  return Node::SPECIES_DEFINITION == node->getNodeType();
}


bool
Node::isParameter(Node *node)
{
  return Node::PARAMETER_DEFINITION == node->getNodeType();
}


bool
Node::isCompartment(Node *node)
{
  return Node::COMPARTMENT_DEFINITION == node->getNodeType();
}


bool
Node::isUnitDefinition(Node *node)
{
  return Node::UNIT_DEFINITION == node->getNodeType();
}


bool
Node::isRule(Node *node)
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
Node::isAssignmentRule(Node *node)
{
  return Node::ASSIGNMENT_RULE == node->getNodeType();
}


bool
Node::isRateRule(Node *node)
{
  return Node::RATE_RULE == node->getNodeType();
}



bool
Node::isReactionDefinition(Node *node)
{
  return Node::REACTION_DEFINITION == node->getNodeType();
}


bool
Node::isKineticLaw(Node *node)
{
  return Node::KINETIC_LAW == node->getNodeType();
}
