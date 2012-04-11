#ifndef __FLUC_COMPILER_TRAFO_WALKER_HH__
#define __FLUC_COMPILER_TRAFO_WALKER_HH__

#include "ast/module.hh"
#include "ast/variabledefinition.hh"
#include "ast/functiondefinition.hh"
#include "ast/reaction.hh"


namespace Fluc {
namespace Ast {
namespace Trafo {

/**
 * This helper class implements the Visitor pattern for the AST. It eases the traversal of the AST
 * a lot. Jut derive from this class an re-implement the required methods.
 *
 * @ingroup trafo
 */
class Walker
{
protected:
  /**
   * Internal method to process scopes, like @c Ast::Module and @c Ast::KineticLaw.
   */
  virtual void handleScope(Ast::Scope *node);


public:
  /**
   * Constructor. Does actually nothing.
   */
  Walker();

  /**
   * Calls handleDefinition for all definitions in the given module.
   *
   * This method is the entry point to process complete modules.
   */
  virtual void handleModule(Ast::Module *node);

  /**
   * Handles a constraint applied on the module.
   *
   * This method dispatches to more specialized methods depending on the type of the constraint.
   */
  virtual void handleConstraint(Ast::Constraint *node);

  /**
   * Handles a algebraic constraint.
   */
  virtual void handleAlgebraicConstraint(Ast::AlgebraicConstraint *node);

  /**
   * Dispatcher for all definitions, also entry point for a definition.
   */
  virtual void handleDefinition(Ast::Definition *node);

  /**
   * This method can be overridden to handle variable definitions.
   */
  virtual void handleVariableDefinition(Ast::VariableDefinition *node);

  /**
   * This method dispatches to specialized method handling rules for variables.
   */
  virtual void handleRule(Ast::Rule *node);

  /**
   * This method can be overridden to handle an assignment rule for a variable definition.
   */
  virtual void handleAssignmentRule(Ast::AssignmentRule *node);

  /**
   * This method can be overridden to handle a rate rule for a variable definition.
   */
  virtual void handleRateRule(Ast::RateRule *node);

  /**
   * This method can be overridden to handle function definitions.
   *
   * This implementation traverses into the function argument definitions and the function
   * body expression.
   */
  virtual void handleFunctionDefinition(Ast::FunctionDefinition *node);

  /**
   * This method can be overridden to handle unit definitions.
   */
  virtual void handleUnitDefinition(Ast::UnitDefinition *node);

  /**
   * This method can be overridden to handle reaction definitions.
   *
   * This implementation traverses into the recatant and production references and the kinetic
   * law definition.
   */
  virtual void handleReaction(Ast::Reaction *node);

  /**
   * This method can be overridden to handle kinetic laws.
   *
   * This implementation traverses into the parameter definitions of the kinetic law and the
   * kinetic law expression.
   */
  virtual void handleKineticLaw(Ast::KineticLaw *node);

  /**
   * This callback dispatches the call to the specialized callbacks for each expression type. This
   * method is also the entry point to process expressions.
   */
  virtual void handleExpression(GiNaC::ex node);

};


}
}
}

#endif // WALKER_HH
