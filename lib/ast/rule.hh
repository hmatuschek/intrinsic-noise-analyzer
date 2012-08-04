#ifndef __FLUC_COMPILER_AST_RULE_HH__
#define __FLUC_COMPILER_AST_RULE_HH__

#include "node.hh"
#include <ginac/ginac.h>


namespace Fluc {
namespace Ast {


/**
 * This is the base class for all rules associated to some variables.
 *
 * @ingroup ast
 */
class Rule : public Node
{
public:
  /** Visitor class for rules. */
  class Visitor { public: virtual void visit(const Rule *rule) = 0; };
  /** Operator class for rules. */
  class Operator { public : virtual void act(Rule *ruls) = 0; };

protected:
  /** Holds the mathematical expression of the rule. The semantic of this expression depends on the
   * type of the rule. */
  GiNaC::ex rule;


protected:
  /** Protected constructor to avoid direct instantiation.
   *
   * @param type Specifies the rule type.
   * @param rule Specifies the rule-expression. The ownership of the expression is passed to the
   *        rule. */
  Rule(Node::NodeType type, GiNaC::ex rule);


public:
  /** Destructor. Also frees the rule-expression. */
  virtual ~Rule();

  /** Returns the rule-expression. The semantic of the returned expression depends on the type
   * of the rule. */
  GiNaC::ex getRule();

  /**  (Re-) Sets the rule expression.
   * @note The replaced rule expression is not desroied, the user is responsible to do that. */
  void setRule(GiNaC::ex rule);

  /** Handles a visitor for this rule. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies a operator on this rule. */
  virtual void apply(Ast::Operator &op);
};



/**
 * Represents a simple assignment rule, that can be attached to a @c Ast::VariableDefinition
 * instance in order to define the value of the variable as the value of the rule expression.
 *
 * @ingroup ast
 */
class AssignmentRule : public Rule
{
public:
  /** Visitor class for assignment rules. */
  class Visitor { public: virtual void visit(const AssignmentRule *rule) = 0; };
  /** Operator class for assignment rules. */
  class Operator { public: virtual void act(AssignmentRule *rule) = 0; };

public:
  /** Constructs a new assignment rule for a variable.
   * @param rule Specifies the rule-expression, which determines the value of the variable directly.
   */
  AssignmentRule(GiNaC::ex rule);

  /** Dumps a string representation the assignment rule into the given stream. */
  virtual void dump(std::ostream &str);

  /** Handles a visitor for an assignment rule. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on an assignment rule. */
  virtual void apply(Ast::Operator &op);
};



/**
 * Represents a so called "rate rule" for a variable.
 *
 * Mathematical speaking, the value of the associated variable is defined by an initial value and
 * a differential equation of first order.
 */
class RateRule : public Rule
{
public:
  /** Visitor class for rate rules. */
  class Visitor { public: virtual void visit(const RateRule *rule) = 0; };
  /** Operator class for rate rules. */
  class Operator { public: virtual void act(RateRule *rule) = 0; };

public:
  /**
   * Constructs a new rate-rule (ODE) for a @c Ast::VariableDefinition instance.
   *
   * @param rule Specifies the ODE for the variable.
   */
  RateRule(GiNaC::ex rule);

  /** Dumps a string representation of the rate-rule into the given stream. */
  void dump(std::ostream &str);

  /** Handles a visitor for a rate rule. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on a rate rule. */
  virtual void apply(Ast::Operator &op);
};


}
}

#endif // __FLUC_COMPILER_AST_RULE_HH__
