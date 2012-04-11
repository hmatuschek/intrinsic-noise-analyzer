#include "substitutioncollector.hh"

using namespace Fluc::Ast;
using namespace Fluc::Ast::Trafo;


SubstitutionCollector::SubstitutionCollector(GiNaC::exmap &table)
  : Walker(), table(table)
{
  // Pass...
}


void
SubstitutionCollector::handleVariableDefinition(Ast::VariableDefinition *node)
{
  // Mark variable for substitution if constant
  if (node->isConst() && node->hasValue())
  {
    this->addSubstitution(node->getSymbol(), node->getValue());
  }
  // Mark variable for substitution if there is an assignmentrule:
  else if (node->hasRule() && Node::isAssignmentRule(node->getRule()))
  {
    AssignmentRule *rule = static_cast<AssignmentRule *>(node->getRule());
    this->addSubstitution(node->getSymbol(), rule->getRule());
  }
}


void
SubstitutionCollector::addSubstitution(const GiNaC::symbol &symbol, GiNaC::ex expression)
{
  // First, perform 'old' substitutions on expression
  expression = expression.subs(this->table);

  // Then substitute symbol == expression on all expressions in table:
  for(GiNaC::exmap::iterator iter = this->table.begin(); iter != this->table.end(); iter++)
  {
    iter->second = iter->second.subs(symbol == expression);
  }

  // Add new subs:
  this->table[symbol] = expression;
}
