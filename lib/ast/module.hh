#ifndef __FLUC_COMPILER_AST_MODULE_HH__
#define __FLUC_COMPILER_AST_MODULE_HH__

#include <map>
#include <list>

#include "scope.hh"
#include "constraint.hh"
#include "unitdefinition.hh"


namespace iNA {
namespace Ast {


/**
 * The module class collects all "code" to be "compiled" from a SBML model, this includes functions,
 * parameter definitions and kinetic laws.
 *
 * @deprecated This class should be unified with the @c Ast::Model class.
 *
 * @ingroup ast
 */
class Module: public Scope
{
public:
  /**
   * Creates an empty module with the given name.
   */
  Module(const std::string &name = "");

  /**
   * Destructor. Destroies also all definitions held by this module.
   */
  virtual ~Module();

  /**
   * Addes the given definition to the module and if the definition is a
   * variable, it also updates the symbol table.
   */
  virtual void addDefinition(Definition *def);

  /**
   * Removes the given definition from the module. The ownership of the definition is transferred
   * to the callee. (The user is responsible to destroy the definition.) If the definition is
   * a @c VariableDefinition, the associated symbol is removed from the symbol table.
   */
  virtual void remDefinition(Definition *def);



  /**
   * Returns the reaction definition by identifier.
   *
   * Is equivalent to call @c getReaction(getSymbol(const std::string &identifier)).
   *
   * @throws SymbolError If the identifier is not associated with a reaction definition.
   */
  Reaction *getReaction(const std::string &identifier);

  /**
   * Returns the reaction definition by identifier.
   *
   * Is equivalent to call @c getReaction(getSymbol(const std::string &identifier)).
   *
   * @throws SymbolError If the identifier is not associated with a reaction definition.
   */
  Reaction * const getReaction(const std::string &identifier) const;

  /**
   * Returns the function definition by identifier.
   *
   * Is equivalent to call @c getFunction(getSymbol(const std::string &identifier));
   *
   * @throws SymbolError If the identifier is not associated with a function.
   */
  FunctionDefinition *getFunction(const std::string &identifier);
};


}
}

#endif // ASTMODULE_HH
