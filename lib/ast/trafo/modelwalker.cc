#include "modelwalker.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Ast::Trafo;


ModelWalker::ModelWalker()
  : Walker()
{
  // Pass...
}


void
ModelWalker::handleModel(Ast::Model *node)
{
  Walker::handleModule(node);
}


void
ModelWalker::handleVariableDefinition(Ast::VariableDefinition *node)
{
  // Dispatch by type:
  switch (node->getNodeType())
  {
  case Ast::Node::SPECIES_DEFINITION:
    return this->handleSpeciesDefinition(static_cast<Ast::Species *>(node));

  case Ast::Node::PARAMETER_DEFINITION:
    return this->handleParameterDefinition(static_cast<Ast::Parameter *>(node));

  case Ast::Node::COMPARTMENT_DEFINITION:
    return this->handleCompartmentDefinition(static_cast<Ast::Compartment *>(node));

  case Ast::Node::FUNCTION_ARGUMENT:
    return this->handleFunctionArgumentDefinition(static_cast<Ast::FunctionArgument *>(node));

  default:
    InternalError err;
    err << "Unknown variable definition type: " << (unsigned int) node->getNodeType();
    throw err;
  }
}


void
ModelWalker::handleSpeciesDefinition(Ast::Species *node)
{
  Walker::handleVariableDefinition(node);
}


void
ModelWalker::handleParameterDefinition(Ast::Parameter *node)
{
  Walker::handleVariableDefinition(node);
}


void
ModelWalker::handleCompartmentDefinition(Ast::Compartment *node)
{
  return Walker::handleVariableDefinition(node);
}


void
ModelWalker::handleFunctionArgumentDefinition(Ast::FunctionArgument *node)
{
  return Walker::handleVariableDefinition(node);
}
