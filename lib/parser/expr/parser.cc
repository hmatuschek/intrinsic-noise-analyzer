#include "parser.hh"
#include <sstream>
#include <parser/lexer.hh>
#include <parser/production.hh>

#include "lexer.hh"
#include "productions.hh"
#include "assembler.hh"
#include "ir.hh"

using namespace iNA;
using namespace iNA::Parser::Expr;


/* ******************************************************************************************** *
 * Implementation of ScopeContext, a context for Ast::Scopes
 * ******************************************************************************************** */
ScopeContext::ScopeContext(const Ast::Scope *model)
  : _scope(model)
{
  // Pass...
}


GiNaC::symbol
ScopeContext::resolve(const std::string &name)
{
  return _scope->getVariable(name)->getSymbol();
}


std::string
ScopeContext::identifier(GiNaC::symbol symbol)
{
  return _scope->getVariable(symbol)->getIdentifier();
}



/* ******************************************************************************************** *
 * Implementation of TableContext, a context for unit tests:
 * ******************************************************************************************** */
TableContext::TableContext()
  : _symbol_table()
{
  // pass...
}

void
TableContext::addSymbol(const std::string &name, GiNaC::symbol symbol)
{
  _symbol_table[name] = symbol;
}

GiNaC::symbol
TableContext::resolve(const std::string &identifier)
{
  if (0 == _symbol_table.count(identifier)) {
    SymbolError err;
    err << "Can not resolve identifier " << identifier << ": Unknown.";
    throw err;
  }
  return _symbol_table[identifier];
}

std::string
TableContext::identifier(GiNaC::symbol symbol)
{
  for (std::map<std::string, GiNaC::symbol>::iterator item=_symbol_table.begin();
       item != _symbol_table.end(); item++)
  {
    if (item->second == symbol) {
      return item->first;
    }
  }

  SymbolError err;
  err << "Can not resolve symbol " << symbol << ": Unknown.";
  throw err;
}



/* ******************************************************************************************** *
 * Implementation of utility functions.
 * ******************************************************************************************** */
GiNaC::ex
Parser::Expr::parseExpression(const std::string &text, Ast::Scope *scope) {
  ScopeContext ctx(scope);
  return parseExpression(text, ctx);
}


GiNaC::ex
Parser::Expr::parseExpression(const std::string &text, Context &ctx)
{
  std::stringstream stream(text);

  iNA::Parser::Lexer lexer(stream);
  lexer.addRule(new Parser::WhiteSpaceTokenRule(T_WHITESPACE));
  lexer.addRule(new Parser::IdentifierTokenRule(T_IDENTIFIER));
  lexer.addTokenName(T_IDENTIFIER, "SYMBOL");
  lexer.addRule(new Parser::IntegerTokenRule(T_INTEGER));
  lexer.addTokenName(T_INTEGER, "INTEGER");
  lexer.addRule(new Parser::FloatTokenRule(T_FLOAT));
  lexer.addTokenName(T_FLOAT, "FLOAT");
  lexer.addRule(new Parser::KeyWordTokenRule(T_COMMA, ","));
  lexer.addTokenName(T_COMMA, "','");
  lexer.addRule(new Parser::KeyWordTokenRule(T_PLUS, "+"));
  lexer.addTokenName(T_PLUS, "'+'");
  lexer.addRule(new Parser::KeyWordTokenRule(T_MINUS, "-"));
  lexer.addTokenName(T_MINUS, "'-'");
  lexer.addRule(new Parser::KeyWordTokenRule(T_TIMES, "*"));
  lexer.addTokenName(T_TIMES, "'*'");
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "**"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "^"));
  lexer.addTokenName(T_POWER, "'**'");
  lexer.addRule(new Parser::KeyWordTokenRule(T_DIVIDE, "/"));
  lexer.addTokenName(T_DIVIDE, "'/'");
  lexer.addRule(new Parser::KeyWordTokenRule(T_LPAR, "("));
  lexer.addTokenName(T_LPAR, "'('");
  lexer.addRule(new Parser::KeyWordTokenRule(T_RPAR, ")"));
  lexer.addTokenName(T_RPAR, "')'");

  lexer.addIgnoredToken(T_WHITESPACE);

  Parser::ConcreteSyntaxTree cst;
  ExpressionGrammar::get()->parse(lexer, cst);

  return Expr::Assembler(ctx, lexer).processExpression(cst[0]);
}


void
Parser::Expr::serializeExpression(
    GiNaC::ex expression, std::ostream &stream, const Ast::Scope *scope, SerializationType stategy)
{
  ScopeContext context(scope);
  serializeExpression(expression, stream, context, stategy);
}


void
Parser::Expr::serializeExpression(
    GiNaC::ex expression, std::ostream &stream, Context &ctx, SerializationType stategy)
{
  // First, create IR from expression
  SmartPtr<Node> node = Node::fromExpression(expression);

  // Apply trafos on expression if needed:
  switch (stategy) {
  case SERIALIZE_PLAIN:
    // No trafo
    break;
  case SERIALIZE_PRETTY:
    // Make expression more readable:
    PrettySerializationTrafo::apply(node);
    break;
  }

  // Serialize IR into stream:
  node->serialize(stream, ctx);
}
