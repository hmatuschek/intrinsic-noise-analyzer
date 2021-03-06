#ifndef __INA_TEST_EXPRESSIONPARSERTEST_HH__
#define __INA_TEST_EXPRESSIONPARSERTEST_HH__

#include "unittest.hh"

namespace iNA {

/** Simple test cases for the expression parser and serialization. */
class ExpressionParserTest : public UnitTest::TestCase
{
public:
  /** Simple test for expression parser on sums. */
  void testParserSum();

  /** Simple test for expression parser on products. */
  void testParserProd();

  /** Simple test for expression parser on powers. */
  void testParserPow();

  /** Simply tests parser and assembler w/o any passes. */
  void testParser();

  /** Simply tests parser and assembler with pretty serialization passes. */
  void testPrettySerialization();

  /** Test parenthesis in serializations. */
  void testParenthesis();

public:
  /** Constructs the test suite. */
  static UnitTest::TestSuite *suite();

  virtual ~ExpressionParserTest() {}
};

}

#endif // __INA_TEST_EXPRESSIONPARSERTEST_HH__
