#include "constraint.hh"

using namespace Fluc::Ast;


Constraint::Constraint(Node::NodeType type)
  : Node(type)
{
  // Pass..
}



/*
 * Implementation of AlgebraicConstraint
 */
AlgebraicConstraint::AlgebraicConstraint(GiNaC::ex rule)
  : Constraint(Node::ALGEBRAIC_CONSTRAINT), constraint(rule)
{
  // Pass.
}


GiNaC::ex
AlgebraicConstraint::getConstraint()
{
  return this->constraint;
}


void
AlgebraicConstraint::setConstraint(GiNaC::ex rule)
{
  this->constraint = rule;
}


void
AlgebraicConstraint::dump(std::ostream &str)
{
  str << " 0 := " << this->constraint;
}
