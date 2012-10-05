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
#include "trafo/variablescaling.hh"


namespace iNA {
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


void __process_model(LIBSBML_CPP_NAMESPACE_QUALIFIER Model *model, ParserContext &ctx);

Ast::FunctionDefinition *__process_function_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER FunctionDefinition *funcdef, ParserContext &ctx);

bool __is_default_unit_redefinition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *node, ParserContext &ctx);

void __process_default_unit_redefinition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *node, ParserContext &ctx);

Ast::UnitDefinition *__process_unit_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *node, ParserContext &ctx);

Ast::ScaledBaseUnit __process_unit(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Unit *unit, ParserContext &ctx);

Ast::VariableDefinition *__process_parameter_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *node, ParserContext &ctx);

Ast::VariableDefinition *__process_compartment_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment *node, ParserContext &ctx, Trafo::VariableScaling &subs);

Ast::VariableDefinition *__process_species_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Species *node, ParserContext &ctx, Trafo::VariableScaling &subs);

Ast::Reaction *__process_reaction(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction *node, ParserContext &ctx);

Ast::KineticLaw *__process_kinetic_law(
  LIBSBML_CPP_NAMESPACE_QUALIFIER KineticLaw *node, ParserContext &ctx);

GiNaC::ex __process_expression(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_const_boolean(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_const_integer(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_const_float(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_arithmetic_operator(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);
GiNaC::symbol __process_symbol(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);
GiNaC::ex __process_function_call(
  const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx);


}
}
}

#endif // __FLUC_PARSER_SBML_ASSEMBLER_HH__
