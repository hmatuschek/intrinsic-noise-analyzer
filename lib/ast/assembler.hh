#ifndef __FLUC_COMPILER_AST_ASSEMBLER_HH__
#define __FLUC_COMPILER_AST_ASSEMBLER_HH__

#include <sbml/SBMLTypes.h>
#include <ginac/ginac.h>

#include "exception.hh"

#include "model.hh"
#include "functiondefinition.hh"
#include "reaction.hh"
#include "unitdefinition.hh"
#include "species.hh"
#include "compartment.hh"
#include "parameter.hh"


namespace Fluc {
namespace Ast {


/**
 * This class traverses a SBML AST representation of mathematical expressions and definitions and
 * assembles a AST representation of a Module, that can be compiled, manipulated or
 * interpreted.
 *
 * @ingroup ast
 */
class Assembler
{
protected:
  /**
   * Holds a weak reference to the current module being processed.
   */
  Model &model;

  /**
   * Implements a stack of variable scopes (nested scopes) variable symboles are resolved
   * top-down along this scopes.
   */
  std::list<Ast::Scope *> scope_stack;


public:
  /**
   * Constructs a new AST assembler for the given module.
   */
  Assembler(Ast::Model &model);

  /**
   * Processes the given model and populates the current AST module with definitions found.
   */
  void processModel(libsbml::Model *model);


protected:
  /**
   * Pushes a scope on the stack:
   */
  void pushScope(Ast::Scope *scope);

  /**
   * Removes a scope from the stack.
   */
  void popScope();

  /**
   * Returns the current scope.
   */
  Ast::Scope *currentScope();

  /**
   * Tries to resolve a species in the current scope.
   */
  Ast::Species *resolveSpecies(const std::string &id);

  /**
   * Tries to resolve a variable in the current scope, if there is no definition in this scope,
   * it ties to resolve the variable in the parent scope.
   */
  Ast::VariableDefinition *resolveVariable(const std::string &id);

  /**
   * Tries to resolve a variable in the given scope.
   */
  Ast::VariableDefinition *resolveVariable(const std::string &id, std::list<Ast::Scope *>::reverse_iterator scope);

  /**
   * Constructs a AST representation of a function definition from the given SBML function
   * definition.
   */
  Ast::FunctionDefinition *processFunctionDefinition(libsbml::FunctionDefinition *funcdef);

  /**
   * Constructs a AST representation of a species definition from the given SBML species.
   */
  Ast::VariableDefinition *processSpeciesDefinition(libsbml::Species *node);

  /**
   * Constructs a AST representation of a parameter definition from the given SBML parameter.
   */
  Ast::VariableDefinition *processParameterDefinition(libsbml::Parameter *node);

  /**
   * Constructs a AST representation of a compartment definition from the given SBML Compartment.
   */
  Ast::VariableDefinition *processCompartmentDefinition(libsbml::Compartment *node);

  /**
   * Returns true, if the unit definition redefines a default unit.
   */
  bool isDefaultUnitRedefinition(libsbml::UnitDefinition *node);

  /**
   * Updates the Module's default units:
   */
  void processDefaultUnitRedefinition(libsbml::UnitDefinition *node);

  /**
   * Constructs a AST representation of a UnitDefinition from the given SBML unit definition.
   */
  Ast::UnitDefinition *processUnitDefinition(libsbml::UnitDefinition *node);

  /**
   * Constructs a ScaledUnit from a SBML unit.
   */
  ScaledBaseUnit processUnit(libsbml::Unit *unit);

  /**
   * Constructs the AST representation of the given SBML reaction.
   */
  Ast::Reaction *processReaction(libsbml::Reaction *node);

  /**
   * Constructs a AST representation of a kinetic law for a SBML reaction using the given
   * reaction id.
   */
  Ast::KineticLaw *processKineticLaw(libsbml::KineticLaw *node);

  /**
   * Dispatcher method to construct an AST representation from any SBML expression (things being
   * evaluated to values).
   */
  GiNaC::ex processExpression(const libsbml::ASTNode *node);

  /**
   * Constructs a @c Ast::ConstBoolean for the given boolean constant.
   */
  GiNaC::ex processConstBoolean(const libsbml::ASTNode *node);

  /**
   * Constructs a @c Ast::ConstInteger for the given integer constant.
   */
  GiNaC::ex processConstInteger(const libsbml::ASTNode *node);

  /**
   * Constructs a @c Ast::ConstFloat for the given floating-point constant.
   */
  GiNaC::ex processConstFloat(const libsbml::ASTNode *node);

  /**
   * Constructs a @c Ast::ArithmeticOperator instance for the given SBML operation.
   */
  GiNaC::ex processArithmeticOperator(const libsbml::ASTNode *node);

  /**
   * Constructs a reference to a symbol (automatically resolved):
   */
  GiNaC::symbol processSymbol(const libsbml::ASTNode *node);

  /**
   * Replaces a function-call with the in-lined expression
   */
  GiNaC::ex processFunctionCall(const libsbml::ASTNode *node);
};


}
}

#endif // __FLUC_COMPILER_AST_ASSEMBLER_HH__
