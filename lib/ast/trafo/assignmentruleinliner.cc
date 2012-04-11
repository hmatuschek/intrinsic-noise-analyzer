#include "assignmentruleinliner.hh"

using namespace Fluc::Ast::Trafo;


AssignmentRuleInliner::AssignmentRuleInliner()
{
  // Pass
}


void
AssignmentRuleInliner::handleModule(Ast::Module *node)
{
  // Before traversing into the module, search for variables with AssignmentRules:
  for (Ast::Module::iterator iter = node->begin(); iter != node->end(); iter++)
  {
    if (! Ast::Node::isVariableDefinition(*iter))
    {
      continue;
    }

    // Cast definition to a variable definition:
    Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*iter);

    // Check if there is a rule assigned
    if (! var->hasRule())
    {
      continue;
    }

    // Is Rule an AssignmentRule?
    if (! Ast::Node::isAssignmentRule(var->getRule()))
    {
      continue;
    }

    // Mark variable to be replaced by assignment rule:
    Ast::AssignmentRule *rule = static_cast<Ast::AssignmentRule *>(var->getRule());
    this->markReplace(var->getSymbol(), rule->getRule());
  }

  // Once all variabled are processed continue with replacement by traversal into module:
  Pass::handleModule(node);
}
