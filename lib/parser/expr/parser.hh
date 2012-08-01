#ifndef __INA_PARSER_EXPR_PARSER_HH__
#define __INA_PARSER_EXPR_PARSER_HH__

#include <ast/ast.hh>


namespace Fluc {
namespace Parser {
namespace Expr {

/**
 * Represents an expression context for symbol resolution.
 */
class Context {
protected:
  /** Holds the stack of scopes for symbol resolution. */
  std::vector<Ast::Scope *> _scope_stack;

public:
  /** Constructs an expression context using the given model as the global namespace/context. */
  Context(Ast::Model *model);

  /** Pushes a (local) scope on the scope stack.  */
  void pushScope(Ast::Scope *scope);
  /** Removes a scope from the scope stack. */
  void popScope();

  /**
   * Resolves the given symbol name using the current context.
   *
   * @throws SymbolError If the name can not be resolved.
   */
  GiNaC::symbol resolve(const std::string &name);
};


/** Parses an expression in the given context.
 * The context is used to resolve symbols of species, compartments and parameters. */
GiNaC::ex parseExpression(const std::string &text, Context &ctx);

}
}
}

#endif
