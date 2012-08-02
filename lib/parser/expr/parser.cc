#include "parser.hh"
#include <sstream>
#include <parser/lexer.hh>
#include <parser/production.hh>

#include "lexer.hh"
#include "productions.hh"
#include "assembler.hh"


using namespace Fluc;
using namespace Fluc::Parser::Expr;


Context::Context(Ast::Scope *model)
  : _scope_stack()
{
  _scope_stack.push_back(model);
}


void
Context::pushScope(Ast::Scope *scope) {
  _scope_stack.push_back(scope);
}

void
Context::popScope() {
  _scope_stack.pop_back();
}



GiNaC::symbol
Context::resolve(const std::string &name)
{
  return resolveVariable(name)->getSymbol();
}


Ast::VariableDefinition *
Context::resolveVariable(const std::string &name)
{
  std::vector<Ast::Scope *>::reverse_iterator scope=_scope_stack.rbegin();
  for ( ; scope != _scope_stack.rend(); scope++) {
    if ((*scope)->hasDefinition(name)) {
      Ast::Definition *def = (*scope)->getDefinition(name);
      Ast::VariableDefinition *var = 0;
      if (0 == (var = dynamic_cast<Ast::VariableDefinition *>(def))) {
        SymbolError err;
        err << "Can not resolve symbol " << name << ": Identifier does not refer to a variable!";
        throw err;
      }
      return var;
    }

    if ((*scope)->isClosed()) {
      SymbolError err;
      err << "Can not resolve symbol " << name << ": Identifier not found.";
      throw err;
    }
  }

  SymbolError err;
  err << "Can not resolve symbol " << name << ": Identifier not found.";
  throw err;
}



GiNaC::ex
Parser::Expr::parseExpression(const std::string &text, Ast::Scope *scope)
{
  std::stringstream stream(text);

  Fluc::Parser::Lexer lexer(stream);
  lexer.addRule(new Parser::WhiteSpaceTokenRule(T_WHITESPACE));
  lexer.addRule(new Parser::IdentifierTokenRule(T_IDENTIFIER));
  lexer.addRule(new Parser::IntegerTokenRule(T_INTEGER));
  lexer.addRule(new Parser::FloatTokenRule(T_FLOAT));
  lexer.addRule(new Parser::KeyWordTokenRule(T_COMMA, ","));
  lexer.addRule(new Parser::KeyWordTokenRule(T_PLUS, "+"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_MINUS, "-"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_TIMES, "*"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "**"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "^"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_DIVIVE, "/"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_LPAR, "("));
  lexer.addRule(new Parser::KeyWordTokenRule(T_RPAR, ")"));
  lexer.addIgnoredToken(T_WHITESPACE);

  Parser::ConcreteSyntaxTree cst;
  ExpressionProduction::get()->parse(lexer, cst);
  ExpressionProduction::get()->notify(lexer, cst);

  return Expr::Assembler(scope, lexer).processExpression(cst);
}
