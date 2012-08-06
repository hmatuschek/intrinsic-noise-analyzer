#ifndef __FLUC_PARSER_SBML_ASSEMBLER_HH__
#define __FLUC_PARSER_SBML_ASSEMBLER_HH__

#include <sbml/SBMLTypes.h>
#include "ast/functiondefinition.hh"
#include "ast/reaction.hh"
#include "ast/unitdefinition.hh"
#include "ast/species.hh"
#include "ast/compartment.hh"
#include "ast/parameter.hh"
#include "exception.hh"
#include "ast/model.hh"


namespace Fluc {
namespace Parser {
namespace Sbml {


/** This class represents a stack of variable scopes that is used to resolve symbol identifiers
 * properly. This class is used internal to parse SBML documents. */
class ParserContext
{
protected:
  /**
   * Holds a weak reference to the current module being processed.
   */
  Ast::Model &_model;

  /**
   * Implements a stack of variable scopes (nested scopes) variable symboles are resolved
   * top-down along this scopes.
   */
  std::list<Ast::Scope *> _scope_stack;


public:
  /**
   * Constructs a new AST assembler for the given module.
   */
  ParserContext(Ast::Model &_model);

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
  Ast::VariableDefinition *resolveVariable(const std::string &id,
                                           std::list<Ast::Scope *>::reverse_iterator scope);

  /** Returns a weak reference to the Ast::Model instance being assembled. */
  Ast::Model &model();
};



void __process_model(libsbml::Model *model, ParserContext &ctx);

Ast::VariableDefinition *__process_species_definition(libsbml::Species *node, ParserContext &ctx);

Ast::FunctionDefinition *__process_function_definition(libsbml::FunctionDefinition *funcdef, ParserContext &ctx);

Ast::VariableDefinition *__process_parameter_definition(libsbml::Parameter *node, ParserContext &ctx);

Ast::VariableDefinition *__process_compartment_definition(libsbml::Compartment *node, ParserContext &ctx);

bool __is_default_unit_redefinition(libsbml::UnitDefinition *node, ParserContext &ctx);

void __process_default_unit_redefinition(libsbml::UnitDefinition *node, ParserContext &ctx);

Ast::UnitDefinition *__process_unit_definition(libsbml::UnitDefinition *node, ParserContext &ctx);

Ast::ScaledBaseUnit __process_unit(libsbml::Unit *unit, ParserContext &ctx);

Ast::Reaction *__process_reaction(libsbml::Reaction *node, ParserContext &ctx);

Ast::KineticLaw *__process_kinetic_law(libsbml::KineticLaw *node, ParserContext &ctx);

GiNaC::ex __process_expression(const libsbml::ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_const_boolean(const libsbml::ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_const_integer(const libsbml::ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_const_float(const libsbml::ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_arithmetic_operator(const libsbml::ASTNode *node, ParserContext &ctx);
GiNaC::symbol __process_symbol(const libsbml::ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_function_call(const libsbml::ASTNode *node, ParserContext &ctx);


}
}
}

#endif // __FLUC_PARSER_SBML_ASSEMBLER_HH__
