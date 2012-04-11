#include "constantfolder.hh"

using namespace Fluc::Ast::Trafo;


ConstantSubstitution::ConstantSubstitution()
{
  // Pass...
}


ConstantSubstitution::ConstantSubstitution(const GiNaC::exmap &table)
{
  // Populate substitution-table with given substitutions.
  this->substitutions.insert(table.begin(), table.end());
}


void
ConstantSubstitution::handleScope(Ast::Scope *node)
{
  // First, populate substitution table with constant variable definitions:
  for (Ast::Scope::iterator iter = node->begin(); iter != node->end(); iter++)
  {
    // skip everything that is not a variable definition:
    if (! Ast::Node::isVariableDefinition(*iter))
    {
      continue;
    }

    Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*iter);

    // Skip non-constant variables:
    if (! var->isConst())
    {
      continue;
    }

    // Skip every thing, that has no value:
    if (! var->hasValue())
    {
      continue;
    }

    // Mark all references to this constant variable to be replaced by the initial value:
    this->markReplace(var->getSymbol(), var->getValue());
  }

  // Then proceed as normal:
  Pass::handleScope(node);
}

