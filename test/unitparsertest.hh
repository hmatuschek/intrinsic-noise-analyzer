#ifndef UNITPARSERTEST_HH
#define UNITPARSERTEST_HH

#include "unittest.hh"

namespace Fluc {

class UnitParserTest : public UnitTest::TestCase
{
public:
  void testSimple();
  void testPow();

public:
  static UnitTest::TestSuite *suite();
};

}

#endif // UNITPARSERTEST_HH