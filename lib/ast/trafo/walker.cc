#include "walker.hh"
#include "exception.hh"


using namespace Fluc::Ast;
using namespace Fluc::Ast::Trafo;


Walker::Walker()
{
}



void
Walker::handleScope(Scope *node)
{
  // Process each definition in scope with "handleDefinition"
  for (Scope::iterator iter = node->begin(); iter != node->end(); iter++)
  {
    this->handleDefinition(*iter);
  }
}


void
Walker::handleModule(Module *node)
{
  this->handleScope(node);

  // process constraints:
  for (Module::constraintIterator iter = node->constraintBegin();
       iter != node->constraintEnd(); iter++)
  {
    this->handleConstraint(*iter);
  }
}


void
Walker::handleConstraint(Constraint *node)
{
  switch (node->getNodeType())
  {
  case Node::ALGEBRAIC_CONSTRAINT:
    return this->handleAlgebraicConstraint(static_cast<AlgebraicConstraint *>(node));

  default:
    InternalError err;
    err << "Can not handle constraint: Unknown constraint type.";
    throw err;
  }
}


void
Walker::handleAlgebraicConstraint(AlgebraicConstraint *node)
{
  // Traverse into expression
  this->handleExpression(node->getConstraint());
}


void
Walker::handleDefinition(Definition *node)
{
  // Dispatch by definition type:
  switch (node->getNodeType())
  {
  case Node::FUNCTION_ARGUMENT:
  case Node::SPECIES_DEFINITION:
  case Node::PARAMETER_DEFINITION:
  case Node::COMPARTMENT_DEFINITION:
    return this->handleVariableDefinition(static_cast<VariableDefinition *>(node));

  case Node::FUNCTION_DEFINITION:
    return this->handleFunctionDefinition(static_cast<FunctionDefinition *>(node));

  case Node::REACTION_DEFINITION:
    return this->handleReaction(static_cast<Reaction *>(node));

  case Node::UNIT_DEFINITION:
    return this->handleUnitDefinition(static_cast<UnitDefinition *>(node));

  default:
    InternalError err;
    err << "Unknwon definition: " << (unsigned int) node->getNodeType();
    throw err;
  }
}


void
Walker::handleVariableDefinition(VariableDefinition *node)
{
  // Process initial value expression if there is one.
  if (node->hasValue())
  {
    this->handleExpression(node->getValue());
  }

  // Process rule if there is one:
  if (node->hasRule())
  {
    this->handleRule(node->getRule());
  }
}


void
Walker::handleRule(Rule *node)
{
  // Dispatch to specialized handler by type of rule:
  switch (node->getNodeType())
  {
  case Node::ASSIGNMENT_RULE:
    return this->handleAssignmentRule(static_cast<AssignmentRule *>(node));

  case Node::RATE_RULE:
    return this->handleRateRule(static_cast<RateRule *>(node));

  default:
    InternalError err;
    err << "Unknown rule type: " << (unsigned int) node->getNodeType();
    throw err;
  }
}


void
Walker::handleAssignmentRule(AssignmentRule *node)
{
  // Traverse into rule expression:
  this->handleExpression(node->getRule());
}


void
Walker::handleRateRule(RateRule *node)
{
  // Traverse into rule expression:
  this->handleExpression(node->getRule());
}


void
Walker::handleFunctionDefinition(FunctionDefinition *node)
{
  // Then, handle function expression:
  this->handleExpression(node->getBody());
}


void
Walker::handleUnitDefinition(Ast::UnitDefinition *node)
{
  // Does nothing...
  node = node;
}


void
Walker::handleReaction(Reaction *node)
{
  // First, process reactants:
  for (Reaction::iterator iter = node->reacBegin(); iter != node->reacEnd(); iter++)
  {
    // Process stoichiometry expression
    this->handleExpression(iter->second);
  }

  // then, process products:
  for (Reaction::iterator iter = node->prodBegin(); iter != node->prodEnd(); iter++)
  {
    // Process stoichiometry expression
    this->handleExpression(iter->second);
  }

  // And finally process kinetic law:
  this->handleKineticLaw(node->getKineticLaw());
}


void
Walker::handleKineticLaw(KineticLaw *node)
{
  // First, process all parameter definitions in compartment:
  this->handleScope(node);

  // Process kinetic law
  this->handleExpression(node->getRateLaw());
}


void
Walker::handleExpression(GiNaC::ex node)
{
  // Pass...
}

