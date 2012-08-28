#include "unitparsertest.hh"
#include "parser/unit/unitparser.hh"


using namespace Fluc;


void
UnitParserTest::testSimple()
{
  Ast::Unit parsed_unit = Parser::Unit::UnitParser::parse(std::string("1e-6 * mole/litre"));

  UT_ASSERT(parsed_unit.isConcentrationUnit());
  UT_ASSERT_EQUAL(parsed_unit.getMultiplier(), 1.0);
  UT_ASSERT_EQUAL(parsed_unit.getScale(), -6.0);
}


void
UnitParserTest::testPow() {
  Ast::Unit true_unit     = Parser::Unit::UnitParser::parse("mole/litre");
  Ast::Unit parsed_unit_2 = Parser::Unit::UnitParser::parse("mole*litre**-1");
  Ast::Unit parsed_unit_1 = Parser::Unit::UnitParser::parse("mole*litre^-1");

  UT_ASSERT(true_unit == parsed_unit_1);
  UT_ASSERT(true_unit == parsed_unit_2);
}

UnitTest::TestSuite *
UnitParserTest::suite() {
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Unit parser test");

  s->addTest(new UnitTest::TestCaller<UnitParserTest>(
               "1e-6 mole/litre", &UnitParserTest::testSimple));
  s->addTest(new UnitTest::TestCaller<UnitParserTest>(
               "mole*litre^-1", &UnitParserTest::testPow));

  return s;
}
