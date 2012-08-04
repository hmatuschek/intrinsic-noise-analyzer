#ifndef __FLUC_COMPILER_AST_VARIABLEDEFINITION_HH__
#define __FLUC_COMPILER_AST_VARIABLEDEFINITION_HH__

#include "definition.hh"
#include "rule.hh"
#include "unitdefinition.hh"


namespace Fluc {
namespace Ast {


/**
 * Represents a variable (species, parameter or compartment) in the AST.
 *
 * @ingroup ast
 */
class VariableDefinition : public Definition
{
public:
  /** Visitor class for variable definitions. */
  class Visitor { public: virtual void visit(const VariableDefinition *var) = 0; };
  /** Operator class of variable definitions. */
  class Operator { public: virtual void act(VariableDefinition *var) = 0; };

protected:
  /** If true, this->value holds the initial value of the variable. */
  bool has_value;

  /** If true, the variable is constant. */
  bool is_const;

  /** Holds the GiNaC symbol representing this expression in GiNaC expressions. */
  GiNaC::symbol symbol;

  /** Holds the (optional) initial-value of the variable/parameter. */
  GiNaC::ex value;

  /** Holds the rule applied to the variable or 0 if there is no rule. */
  Rule *rule;

  /** Holds a unit of the variable. */
  Unit unit;


public:
  /**
   * Constructs a variable definition without type and initial value expression.
   *
   * @param type Specifies the node type (parameter, species, ...)
   * @param id Specifies the identifier/name of the variable.
   * @param unit Specifies the unit of the variable.
   * @param is_const Specifies if the variable is a constant.
   */
  VariableDefinition(Node::NodeType type, const std::string &id, const Unit &unit, bool is_const=false);

  /**
   * Constructs a (complete) variable definition.
   *
   * @param type Specifies the node type (parameter, species, ...)
   * @param id Specifies the identifier/name of the variable.
   * @param value Specifies the initial value expression. The ownership of the expression is passed
   *        to the variable definition instance. If the variable is destroyed, the initial value
   *        expression is also freed.
   * @param unit Specifies the unit of the variable.
   * @param name Specifies the optional display-name of the variable.
   * @param is_const Specifies if the variable is a constant.
   */
  VariableDefinition(Node::NodeType type, const std::string &id, GiNaC::ex value,
                     const Unit &unit, const std::string &name, bool is_const=false);

  /**
   * Constructs a variable definition with attached rule.
   *
   * @param type Specifies the node type (parameter, species, ...)
   * @param id Specifies the identifier/name of the variable.
   * @param value Specifies the initial value expression. The ownership of the expression is passed
   *        to the variable definition instance. If the variable is destroyed, the initial value
   *        expression is also freed.
   * @param rule Specifies the rule applied to the variable.
   * @param unit Specifies the unit of the variable.
   * @param is_const Specifies if the variable is a constant.
   */
  VariableDefinition(Node::NodeType type, const std::string &id, GiNaC::ex value, Rule *rule,
                     const Unit &unit, bool is_const=false);

  /**
   * Destructor. Also frees the initial value expression, if present.
   */
  virtual ~VariableDefinition();

  /**
   * Returns true, if the variable is constant.
   */
  bool isConst();

  /**
   * Retunrs true, if the variable definition has an initial value expression.
   */
  bool hasValue();

  /**
   * Retruns the initial value expression, if present. Otherwise null is returned.
   */
  GiNaC::ex getValue();

  /**
   * (re-) Sets the initial value expression.
   */
  void setValue(GiNaC::ex value);

  /**
   * Returns the symbol associated with the definition.
   */
  const GiNaC::symbol &getSymbol() const;

  /**
   * Retunrs true, if there is a rule attached to the variable.
   */
  bool hasRule() const;

  /**
   * Retunrs the rule attached to the variable.
   */
  Rule *getRule();

  /**
   * (Re-) Sets the attached rule for the variable.
   */
  void setRule(Rule *rule);

  /**
   * Returns true, if the variable as a unit assigned.
   */
  bool isDimensionLess() const;

  /**
   * Returns the unit of the variable.
   */
  const Unit &getUnit() const;

  /**
   * Resets the unit of the variable.
   */
  void setUnit(const Unit &unit);

  /**
   * Just dumps a simple string representation of the variable definition into the given stream.
   */
  virtual void dump(std::ostream &str);

  /** Handles a visitor for the variable definition. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the variable definition. */
  virtual void apply(Ast::Operator &op);
};


}
}

#endif // VARIABLEDEFINITION_HH
