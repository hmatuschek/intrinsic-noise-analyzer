#include "constraint.hh"

using namespace Fluc::Ast;


Constraint::Constraint(Node::NodeType type)
  : Node(type)
{
  // Pass..
}

void
Constraint::accept(Ast::Visitor &visitor) const
{
  if (Constraint::Visitor *vis = dynamic_cast<Constraint::Visitor *>(&visitor)) {
    vis->visit(this);
  } else {
    Node::accept(visitor);
  }
}

void
Constraint::apply(Ast::Operator &op) {
  if (Constraint::Operator *con_op = dynamic_cast<Constraint::Operator *>(&op)) {
    con_op->act(this);
  } else {
    Node::apply(op);
  }
}


/*
 * Implementation of AlgebraicConstraint
 */
AlgebraicConstraint::AlgebraicConstraint(GiNaC::ex rule)
  : Constraint(Node::ALGEBRAIC_CONSTRAINT), constraint(rule)
{
  // Pass.
}


void
AlgebraicConstraint::accept(Ast::Visitor &visitor) const
{
  if (AlgebraicConstraint::Visitor *con_vis = dynamic_cast<AlgebraicConstraint::Visitor *>(&visitor)) {
    con_vis->visit(this);
  } else {
    Constraint::accept(visitor);
  }
}

void
AlgebraicConstraint::apply(Ast::Operator &op)
{
  if (AlgebraicConstraint::Operator *con_op = dynamic_cast<AlgebraicConstraint::Operator *>(&op)) {
    con_op->act(this);
  } else {
    Constraint::apply(op);
  }
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
