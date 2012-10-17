#include "expressionparsertest.hh"
#include <parser/expr/parser.hh>

using namespace iNA;


void
ExpressionParserTest::testParser() {
  Parser::Expr::TableContext ctx;
  ctx.addSymbol("a", GiNaC::symbol("a"));
  ctx.addSymbol("b", GiNaC::symbol("b"));
  ctx.addSymbol("c", GiNaC::symbol("c"));

  std::stringstream buffer;
  buffer << "(a-1)*(-c)*(a+b)^-1";

}
