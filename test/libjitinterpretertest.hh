#ifndef __FLUC_TEST_LIBJITINTERPRETERTEST_HH__
#define __FLUC_TEST_LIBJITINTERPRETERTEST_HH__

#include "unittest.hh"

namespace Fluc {

class LibJitInterpreterTest : public UnitTest::TestCase
{
public:
  void testComplexSum();
  void testComplexProd();
  void testComplexPow();

public:
  static UnitTest::TestSuite *suite();
};

}

#endif // __FLUC_TEST_LIBJITINTERPRETERTEST_HH__
