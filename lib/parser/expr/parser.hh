#ifndef __INA_PARSER_EXPR_PARSER_HH__
#define __INA_PARSER_EXPR_PARSER_HH__

#include "../../ast/ast.hh"


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
  virtual std::string identifier(GiNaC::symbol symbol) const = 0;
};


/** Implementes symbol resolution using @c iNA::Ast::Scope instances. */
class ScopeContext : public Context
{
protected:
  /** Holds the scope of the context to be used to resolve symbols. */
  const Ast::Scope *_scope;

public:
  /** Constructs an expression context using the given model as the global namespace/context. */
  ScopeContext(const Ast::Scope *model);

  /** Resolves the given symbol name using the current context.
   * @throws SymbolError If the name can not be resolved. */
  virtual GiNaC::symbol resolve(const std::string &name);

  /** Resolves the given GiNaC symbol to its identifier. */
  virtual std::string identifier(GiNaC::symbol symbol) const;
};


/** Defined different serialization methods used by @c serializeExpression to
 * determine the transformations performed before serializing the expression. */
typedef enum {
  SERIALIZE_PLAIN,   ///< Direct serialization.
  SERIALIZE_PRETTY   ///< Tries to serialize expressions more readable.
} SerializationType;


/** A simple parser context mainly for testing. It holds a simple identifier->symbol table to
 * resolve symbol names. */
class TableContext : public Context {
protected:
  /** Holds the identifier -> symbol table. */
  std::map<std::string, GiNaC::symbol> _symbol_table;

public:
  /** Constructor. */
  TableContext();
  /** Adds a symbol to the table. */
  void addSymbol(const std::string &name, GiNaC::symbol symbol);
  /** Resolves a symbol identifier. */
  GiNaC::symbol resolve(const std::string &identifier);
  /** Resolves a symbol. */
  std::string identifier(GiNaC::symbol symbol) const;
};


/** Parses an expression in the given variable scope.
 * The scope (context) is used to resolve symbols of species, compartments and parameters.
 * @ingroup modelio */
GiNaC::ex parseExpression(const std::string &text, Context &scope);

/** Parses an expression in the given variable scope.
 * The scope (context) is used to resolve symbols of species, compartments and parameters.
 * @ingroup modelio */
GiNaC::ex parseExpression(const std::string &text, Ast::Scope *scope);

/** Serializes the given expression into the given stream using the given context. */
void serializeExpression(GiNaC::ex expression, std::ostream &stream, const Context &ctx,
                         SerializationType stategy=SERIALIZE_PRETTY);

/** Serializes the given expression into the given stream using the given context. */
void serializeExpression(GiNaC::ex expression, std::ostream &stream, const Ast::Scope *scope,
                         SerializationType stategy=SERIALIZE_PRETTY);

}
}
}

#endif
