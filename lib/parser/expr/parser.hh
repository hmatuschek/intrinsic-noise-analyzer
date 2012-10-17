#ifndef __INA_PARSER_EXPR_PARSER_HH__
#define __INA_PARSER_EXPR_PARSER_HH__

#include <ast/ast.hh>


namespace iNA {
namespace Parser {
namespace Expr {

/** Represents an expression context for symbol resolution.
 * This class is used by all assemblers to perfrom symbol resolution in nested contexts. */
class Context {
public:
  /** Resolves a given identifier to its symbol or throws a @c SymbolError exception if
   * the symbol can not be resolved. */
  virtual GiNaC::symbol resolve(const std::string &identifier) = 0;
  /** Returns the idenfitier for the given symbol. This is used for serializing expressions into
   * text. */
  virtual std::string identifier(GiNaC::symbol symbol) = 0;
};


/** Implementes symbol resolution using @c iNA::Ast::Scope instances. */
class ScopeContext : public Context
{
protected:
  /** Holds the stack of scopes for symbol resolution.
   * @deprecated Scopes are nested anyway. */
  std::vector<Ast::Scope *> _scope_stack;

public:
  /** Constructs an expression context using the given model as the global namespace/context. */
  ScopeContext(Ast::Scope *model);

  /** Pushes a (local) scope on the scope stack.
   * @deprecated Scopes are nested anyway. */
  void pushScope(Ast::Scope *scope);
  /** Removes a scope from the scope stack.
   * @deprecated Scopes are nested anyway. */
  void popScope();

  /** Resolves the given symbol name using the current context.
   * @throws SymbolError If the name can not be resolved. */
  virtual GiNaC::symbol resolve(const std::string &name);

  /** Resolves the given GiNaC symbol to its identifier. */
  virtual std::string identifier(GiNaC::symbol symbol);

  /** Resolves the given variable name using the current context.
   * @deprecated Use resolve() instead.
   * @throws SymbolError If the name can not be resolved.
   */
  Ast::VariableDefinition *resolveVariable(const std::string &name);
};


/** Parses an expression in the given variable scope.
 * The scope (context) is used to resolve symbols of species, compartments and parameters.
 * @ingroup modelio */
GiNaC::ex parseExpression(const std::string &text, Context &scope);

/** Parses an expression in the given variable scope.
 * The scope (context) is used to resolve symbols of species, compartments and parameters.
 * @ingroup modelio */
GiNaC::ex parseExpression(const std::string &text, Ast::Scope *scope);

}
}
}

#endif
