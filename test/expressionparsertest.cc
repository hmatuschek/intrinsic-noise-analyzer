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

  // Parse expression;
  GiNaC::ex expression_1 = Parser::Expr::parseExpression(buffer.str(), ctx);
  // Serialize expression back into string:
  buffer.str(""); Parser::Expr::serializeExpression(expression_1, buffer, ctx);
  // And parse serialized expression again:
  GiNaC::ex expression_2 = Parser::Expr::parseExpression(buffer.str(), ctx);

  UT_ASSERT_EQUAL(expression_1.expand(), expression_2.expand());
}


void
ExpressionParserTest::testPrettySerialization() {
  Parser::Expr::TableContext ctx;
  ctx.addSymbol("a", GiNaC::symbol("a"));
  ctx.addSymbol("b", GiNaC::symbol("b"));
  ctx.addSymbol("c", GiNaC::symbol("c"));

  std::stringstream buffer;
  buffer << "(a-1)*(-c)*(a+b)^-1";

  // Parse expression;
  GiNaC::ex expression_1 = Parser::Expr::parseExpression(buffer.str(), ctx);
  // Serialize expression back into string:
  buffer.str("");
  Parser::Expr::serializeExpression(expression_1, buffer, ctx, Parser::Expr::SERIALIZE_PRETTY);
  // And parse serialized expression again:
  GiNaC::ex expression_2 = Parser::Expr::parseExpression(buffer.str(), ctx);

  UT_ASSERT_EQUAL(expression_1.expand(), expression_2.expand());
}

void
ExpressionParserTest::testParenthesis() {
  Parser::Expr::TableContext ctx;
  ctx.addSymbol("a", GiNaC::symbol("a"));
  ctx.addSymbol("b", GiNaC::symbol("b"));
  ctx.addSymbol("c", GiNaC::symbol("c"));
  GiNaC::ex expression_1, expression_2;
  std::stringstream buffer;

  buffer << "a-(b-c)";
  expression_1 = Parser::Expr::parseExpression(buffer.str(), ctx); buffer.str("");
  Parser::Expr::serializeExpression(expression_1, buffer, ctx, Parser::Expr::SERIALIZE_PRETTY);
  expression_2 = Parser::Expr::parseExpression(buffer.str(), ctx); buffer.str("");
  UT_ASSERT_EQUAL(expression_1.expand(), expression_2.expand());

  buffer << "a-b-c";
  expression_1 = Parser::Expr::parseExpression(buffer.str(), ctx); buffer.str("");
  Parser::Expr::serializeExpression(expression_1, buffer, ctx, Parser::Expr::SERIALIZE_PRETTY);
  expression_2 = Parser::Expr::parseExpression(buffer.str(), ctx); buffer.str("");
  UT_ASSERT_EQUAL(expression_1.expand(), expression_2.expand());
}


UnitTest::TestSuite *
ExpressionParserTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests custom expresion parser.");

  s->addTest(new UnitTest::TestCaller<ExpressionParserTest>(
               "simple parser", &ExpressionParserTest::testParser));

  s->addTest(new UnitTest::TestCaller<ExpressionParserTest>(
               "pretty serialization", &ExpressionParserTest::testPrettySerialization));

  s->addTest(new UnitTest::TestCaller<ExpressionParserTest>(
               "parenthesis serialization", &ExpressionParserTest::testParenthesis));

  return s;
}
