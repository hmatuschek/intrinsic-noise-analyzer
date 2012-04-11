#ifndef __FLUC_TEST_MATHTEST_HH__
#define __FLUC_TEST_MATHTEST_HH__

#include "unittest.hh"

namespace Fluc {


class MathTest : public UnitTest::TestCase
{
public:
  void testErf();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // MATHTEST_HH
