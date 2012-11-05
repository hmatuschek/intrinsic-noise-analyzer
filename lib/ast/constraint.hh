#ifndef CONSTRAINT_HH
#define CONSTRAINT_HH

#include "node.hh"
#include <ginac/ginac.h>


namespace iNA {
namespace Ast {


/**
 * Base class custom all constraints.
 *
 * @ingroup ast
 */
class Constraint : public Node
{
public:
  /** Visitor class custom constraints. */
  class Visitor {
  public:
    /** Implements the visitor. */
    virtual void visit(const Ast::Constraint *constraint) = 0;
  };

  /** Operator class custom constaints. */
  class Operator {
  public:
    /** Handles the operator. */
    virtual void act(Ast::Constraint *constraint) = 0;
  };

protected:
  /** Protected constructor to avoid direct instantiation. */
  Constraint(Node::NodeType type);

public:
  /** Accepts a visitor custom the constraint. */
  virtual void accept(Ast::Visitor &visitor) const;
  /** Applies an operator on the constaints. */
  virtual void apply(Ast::Operator &op);
};



/**
 * Represents an algebraic constraint to be applied to a variable.
 *
 * @ingroup ast
 */
class AlgebraicConstraint : public Constraint
{
public:
  class Visitor { public: virtual void visit(const AlgebraicConstraint *node) = 0; };
  class Operator { public: virtual void act(AlgebraicConstraint *node) = 0; };

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

  virtual void accept(Ast::Visitor &visitor) const;
  virtual void apply(Ast::Operator &op);

  /** Returns the constraint expression. */
  GiNaC::ex getConstraint() const;

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
