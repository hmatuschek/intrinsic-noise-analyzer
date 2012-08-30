#include "pass.hh"
#include "exception.hh"

using namespace iNA::Ast;
using namespace iNA::Ast::Trafo;


Pass::Pass()
{
  // Pass...
}


Pass::Pass(const GiNaC::exmap &table)
  : substitutions(table)
{
  // pass...
}


void
Pass::markReplace(Node *a, Node *b)
{
  // First, store subtitution in table:
  this->replacements[a] = b;

  // If "a" is a definition -> store it also into translation table:
  if (Node::isDefinition(a))
  {
    this->translations[a] = b;
  }
}


void
Pass::markReplace(GiNaC::ex a, GiNaC::ex b)
{
  this->substitutions[a] = b.subs(this->substitutions);
}


void
Pass::handleModule(Module *node)
{
  // First traverse into definitions and constraints:
  Walker::handleModule(node);

  this->cleanupModule(node);
}


void
Pass::cleanupModule(Module *node)
{
  // Cleanup of definitions is performed by handle/cleanupScope

  // Check if one of the constraints is replaced:
  for (Module::constraintIterator iter = node->constraintBegin();
       iter != node->constraintEnd(); iter++)
  {
    // Check if constraint is replaced:
    std::map<Node *, Node *>::iterator item = this->replacements.find(*iter);
    if (this->replacements.end() == item)
    {
      continue;
    }

    // Check if replacement is a Constraint instance:
    if (! Node::isConstraint(item->second))
    {
      InternalError err;
      err << "Can not replace constraint with none: "
          << (unsigned int) item->second->getNodeType();
      throw err;
    }

    // Replace constraint with cast:
    *iter = static_cast<Constraint *>(item->second);

    // Destroy 'old' constraint:
    delete item->first;

    // Remove replacement from table:
    this->replacements.erase(item);
  }
}


void
Pass::handleAlgebraicConstraint(AlgebraicConstraint *node)
{
  // First, traverse
  Walker::handleAlgebraicConstraint(node);

  this->cleanupAlgebraicConstraint(node);
}


void
Pass::cleanupAlgebraicConstraint(AlgebraicConstraint *node)
{
  // Replace expression with cast:
  node->setConstraint(node->getConstraint().subs(this->substitutions));
}


void
Pass::handleScope(Scope *node)
{
  // First, traverse into the scope (module)
  Walker::handleScope(node);

  this->cleanupScope(node);
}


void
Pass::cleanupScope(Scope *node)
{
  // For each definition in scope:
  for (Scope::iterator iter = node->begin(); iter != node->end(); iter++)
  {
    // Check if definition is marked to be replaced:
    std::map<Node *, Node *>::iterator item = this->replacements.find(*iter);
    if (this->replacements.end() != item)
    {
      // Check if replacement is a Definition:
      if (! Node::isDefinition(item->second))
      {
        InternalError err;
        err << "Can not replace a Definition with: "
            << (unsigned int) item->second->getNodeType();
        throw err;
      }

      // replace the 'old' node with the 'new' one:
      *iter = dynamic_cast<Definition *>(item->second);

      // Destroy 'old' node:
      delete item->first;

      // Remove replacement from table:
      this->replacements.erase(item);
    }
  }
}


void
Pass::handleFunctionDefinition(FunctionDefinition *node)
{
  // First traverse into function definition:
  Walker::handleFunctionDefinition(node);

  this->cleanupFunctionDefinition(node);
}


void
Pass::cleanupFunctionDefinition(FunctionDefinition *node)
{
  // Cleanup function arguments (scope):
  this->cleanupScope(node);

  // If function expression is replaced:
  node->setBody(node->getBody().subs(this->substitutions));
}


void
Pass::handleReaction(Reaction *node)
{
  // First, traverse into reaction definition:
  Walker::handleReaction(node);

  this->cleanupReaction(node);
}


void
Pass::cleanupReaction(Reaction *node)
{
  // Check for cleanup in reactants:
  for (Reaction::iterator iter = node->reacBegin(); iter != node->reacEnd(); iter++)
  {
    // Replace stoichiometric expression:
    iter->second = iter->second.subs(this->substitutions);
  }

  // Check for cleanup in products:
  for (Reaction::iterator iter = node->prodBegin(); iter != node->prodEnd(); iter++)
  {
    // Replace stoichiometric expression:
    iter->second = iter->second.subs(this->substitutions);
  }

  // Check if kinetic law is replaced:
  std::map<Node *, Node *>::iterator item = this->replacements.find(node->getKineticLaw());
  if (this->replacements.end() != item)
  {
    // Check if replacement is a kinetic law:
    if (! Node::isKineticLaw(item->second))
    {
      InternalError err;
      err << "Can not replace kinetic law with: " << (unsigned int) item->second->getNodeType();
      throw err;
    }

    // Replace kinetic law with cast
    node->setKineticLaw(dynamic_cast<KineticLaw *>(item->second));

    // Free 'old' law:
    delete item->first;

    // Remove replacement from table:
    this->replacements.erase(item);
  }
}


void
Pass::handleKineticLaw(KineticLaw *node)
{
  // First, process all children:
  Walker::handleKineticLaw(node);

  this->cleanupKineticLaw(node);
}


void
Pass::cleanupKineticLaw(KineticLaw *node)
{
  // A KineticLawDefinition is a Scope -> cleanupScope
  this->cleanupScope(node);

  // Perform some substitutions on the rate law.
  node->setRateLaw(node->getRateLaw().subs(this->substitutions));
}


void
Pass::handleVariableDefinition(VariableDefinition *node)
{
  // First, traverse into initial value:
  Walker::handleVariableDefinition(node);

  this->cleanupVariableDefinition(node);
}


void
Pass::cleanupVariableDefinition(VariableDefinition *node)
{
  // Check if there is a initial value definition:
  if (node->hasValue())
  {
    node->setValue(node->getValue().subs(this->substitutions));
  }

  // Check if variable definition has a rule assigned:
  if (node->hasRule())
  {
    // Check if rule is replaced:
    std::map<Node *, Node *>::iterator item = this->replacements.find(node->getRule());
    if (this->replacements.end() != item)
    {
      // Check if replacement is a rule instance:
      if (! Node::isRule(item->second))
      {
        InternalError err;
        err << "Can not replace rule with none: " << (unsigned int) item->second->getNodeType();
        throw err;
      }

      // Replace rule with cast:
      node->setRule(static_cast<Rule *>(item->second));

      // destroy replaced rule:
      delete item->first;

      // Remove replacement from table:
      this->replacements.erase(item);
    }
  }
}


void
Pass::handleRule(Rule *node)
{
  // First, traverse into rule:
  Walker::handleRule(node);

  this->cleanupRule(node);
}


void
Pass::cleanupRule(Rule *node)
{
  // substitute the rule-expression:
  node->setRule(node->getRule().subs(this->substitutions));
}
