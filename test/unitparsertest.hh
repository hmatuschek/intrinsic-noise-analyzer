#ifndef UNITPARSERTEST_HH
#define UNITPARSERTEST_HH

#include "unittest.hh"

namespace iNA {

class UnitParserTest : public UnitTest::TestCase
{
public:
  virtual ~UnitParserTest(){};
  void testSimple();
  void testPow();

public:
  static UnitTest::TestSuite *suite();
};

}

#endif // UNITPARSERTEST_HH
