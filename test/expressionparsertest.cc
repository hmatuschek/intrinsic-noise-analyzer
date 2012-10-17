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
  std::cerr << "Serialized expression: " << buffer.str();
  // And parse serialized expression again:
  GiNaC::ex expression_2 = Parser::Expr::parseExpression(buffer.str(), ctx);

  UT_ASSERT_EQUAL(expression_1, expression_2);
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
  buffer.str(""); Parser::Expr::serializeExpression(expression_1, buffer, ctx,
                                                    Parser::Expr::SERIALIZE_PRETTY);
  std::cerr << "Serialized expression: " << buffer.str();
  // And parse serialized expression again:
  GiNaC::ex expression_2 = Parser::Expr::parseExpression(buffer.str(), ctx);

  UT_ASSERT_EQUAL(expression_1, expression_2);
}

UnitTest::TestSuite *
ExpressionParserTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests for expresion parser.");

  s->addTest(new UnitTest::TestCaller<ExpressionParserTest>(
               "simple parser test", &ExpressionParserTest::testParser));

  s->addTest(new UnitTest::TestCaller<ExpressionParserTest>(
               "pretty serialization test", &ExpressionParserTest::testPrettySerialization));

  return s;
}
