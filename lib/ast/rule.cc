#include "rule.hh"

using namespace Fluc::Ast;


Rule::Rule(Node::NodeType type, GiNaC::ex rule)
  : Node(type), rule(rule)
{
  // pass
}


Rule::~Rule()
{
}


GiNaC::ex
Rule::getRule()
{
  return this->rule;
}


void
Rule::setRule(GiNaC::ex rule)
{
  this->rule = rule;
}



/*
 * Implementation of AssignmentRule
 */
AssignmentRule::AssignmentRule(GiNaC::ex rule)
  : Rule(Node::ASSIGNMENT_RULE, rule)
{
  // Pass.
}


void
AssignmentRule::dump(std::ostream &str)
{
  str << " := " << this->rule;
}


/*
 * Implementation of RateRule
 */
RateRule::RateRule(GiNaC::ex rule)
  : Rule(Node::RATE_RULE, rule)
{
  // Pass.
}


void
RateRule::dump(std::ostream &str)
{
  str << " := " << this->rule;
}
