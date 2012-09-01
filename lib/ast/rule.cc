#include "rule.hh"

using namespace iNA::Ast;


Rule::Rule(Node::NodeType type, GiNaC::ex rule)
  : Node(type), rule(rule)
{
  // pass
}


Rule::~Rule()
{
}


GiNaC::ex
Rule::getRule() const
{
  return this->rule;
}


void
Rule::setRule(GiNaC::ex rule)
{
  this->rule = rule;
}


void
Rule::accept(Ast::Visitor &visitor) const
{
  if (Rule::Visitor *rule_vis = dynamic_cast<Rule::Visitor *>(&visitor)) {
    rule_vis->visit(this);
  } else {
    Node::accept(visitor);
  }
}

void
Rule::apply(Ast::Operator &op)
{
  if (Rule::Operator *rule_op = dynamic_cast<Rule::Operator *>(&op)) {
    rule_op->act(this);
  } else {
    Node::apply(op);
  }
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


void
AssignmentRule::accept(Ast::Visitor &visitor) const
{
  if (AssignmentRule::Visitor *rule_vis = dynamic_cast<AssignmentRule::Visitor *>(&visitor)) {
    rule_vis->visit(this);
  } else {
    Rule::accept(visitor);
  }
}

void
AssignmentRule::apply(Ast::Operator &op)
{
  if (AssignmentRule::Operator *rule_op = dynamic_cast<AssignmentRule::Operator *>(&op)) {
    rule_op->act(this);
  } else {
    Rule::apply(op);
  }
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


void
RateRule::accept(Ast::Visitor &visitor) const
{
  if (RateRule::Visitor *rule_vis = dynamic_cast<RateRule::Visitor *>(&visitor)) {
    rule_vis->visit(this);
  } else {
    Rule::accept(visitor);
  }
}


void
RateRule::apply(Ast::Operator &op)
{
  if (RateRule::Operator *rule_op = dynamic_cast<RateRule::Operator *>(&op)) {
    rule_op->act(this);
  } else {
    Rule::apply(op);
  }
}
