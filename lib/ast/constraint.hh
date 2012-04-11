#ifndef CONSTRAINT_HH
#define CONSTRAINT_HH

#include "node.hh"
#include <ginac/ginac.h>


namespace Fluc {
namespace Ast {


/**
 * Base class for all constraints.
 *
 * @ingroup ast
 */
class Constraint : public Node
{
protected:
  /**
   * Protected constructor to avoid direct instantiation.
   */
  Constraint(Node::NodeType type);
};



/**
 * Represents an algebraic constraint to be applied to a variable.
 *
 * @ingroup ast
 */
class AlgebraicConstraint : public Constraint
{
protected:
  /**
   * Holds the expression defining the constraint on the used variables.
   */
  GiNaC::ex constraint;


public:
  /**
   * Constructs an algebraic constraint, that applies to all variables.
   *
   * @param rule Specifies the expression to solve. The ownership of the expression is passed to
   *        the rule.
   */
  AlgebraicConstraint(GiNaC::ex rule);

  /**
   * Returns the constraint expression.
   */
  GiNaC::ex getConstraint();

  /**
   * (Re-) Sets the constraint expression. The replaced expression is not freed.
   */
  void setConstraint(GiNaC::ex expr);

  /**
   * Simply dumps a string representation of the constraint into the given stream.
   */
  virtual void dump(std::ostream &str);
};

}
}

#endif // CONSTRAINT_HH
