#ifndef __FLUC_COMPILER_TRAFO_PASS_HH__
#define __FLUC_COMPILER_TRAFO_PASS_HH__

#include "walker.hh"


namespace Fluc {
namespace Ast {
namespace Trafo {


/**
 * A pass extends the @c Walker by performing postponed replacements once a Node was handled.
 *
 * @ingroup ast
 */
class Pass : public Walker
{
protected:
  /**
   * Holds the substitutions to make. Once the substitution has been made, the substituion is
   * removed from this table. This table holds only replacements for @c Ast::Node instances.
   * Substitutions in expressions are handled by the exmap.
   */
  std::map<Ast::Node *, Ast::Node *> replacements;

  /**
   * Holds the substitutions of GiNaC expressions.
   */
  GiNaC::exmap substitutions;

  /**
   * This table holds the stubstitutions of definitions, used to re-link references to substituted
   * definitions.
   */
  std::map<Ast::Node *, Ast::Node *> translations;


protected:
  /**
   * Helper method to add substitutions to the tables.
   */
  void markReplace(Ast::Node *a, Ast::Node *b);

  /**
   * Helper function to mark substitutions in expressions.
   */
  void markReplace(GiNaC::ex a, GiNaC::ex b);


public:
  /**
   * Constructs a pass.
   */
  Pass();

  /**
   * Constructs a simple pass with a predefined substitution table.
   */
  Pass(const GiNaC::exmap &table);


  /**
   * Traverses the module and performs a cleanup of the constraints if replacements are left.
   */
  virtual void handleModule(Ast::Module *node);

  /**
   * Performs the actual replacements on the module constraints.
   */
  virtual void cleanupModule(Ast::Module *node);

  /**
   * Traverses the AlgebraicConstraint.
   */
  virtual void handleAlgebraicConstraint(Ast::AlgebraicConstraint *node);

  /**
   * Performs the replacement of the constraint expression.
   */
  virtual void cleanupAlgebraicConstraint(Ast::AlgebraicConstraint *node);

  /**
   * Traverses into all definitions of the scope.
   */
  virtual void handleScope(Ast::Scope *node);

  /**
   * Performs the substitution of the definitions of the scope.
   */
  virtual void cleanupScope(Ast::Scope *node);

  /**
   * Traverses into the function body and function arguments.
   */
  virtual void handleFunctionDefinition(Ast::FunctionDefinition *node);

  /**
   * Performs the substitution of function body and function arguments.
   */
  virtual void cleanupFunctionDefinition(Ast::FunctionDefinition *node);

  /**
   * Traverses into the stoichiometric expressions and kinetic law of the reaction.
   */
  virtual void handleReaction(Ast::Reaction *node);

  /**
   * Performs the substitution of stoichiometric expressions and the kinetic law.
   */
  virtual void cleanupReaction(Ast::Reaction *node);

  /**
   * Traverses into the kinetic law.
   */
  virtual void handleKineticLaw(Ast::KineticLaw *node);

  /**
   * Performs the substitution on the kinetic law.
   */
  virtual void cleanupKineticLaw(Ast::KineticLaw *node);

  /**
   * Traverses into the variable definition.
   */
  virtual void handleVariableDefinition(Ast::VariableDefinition *node);

  /**
   * Performs the substiturions of child-elements of the variable definition.
   */
  virtual void cleanupVariableDefinition(Ast::VariableDefinition *node);

  /**
   * Traverses into the rule.
   */
  virtual void handleRule(Ast::Rule *node);

  /**
   * Performs the substitution of the rule-expression of the rule.
   */
  virtual void cleanupRule(Ast::Rule *node);
};


}
}
}

#endif // PASS_HH
