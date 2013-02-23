#ifndef STEADYSTATETEST_HH
#define STEADYSTATETEST_HH

#include "unittest.hh"


namespace iNA {

/** Checks the SSE steady state analysis. */
class SteadyStateTest : public UnitTest::TestCase
{
public:
  void testEnzymeKineticsRE();
  void testEnzymeKineticsLNA();
  void testEnzymeKineticsIOS();

public:
  static UnitTest::TestSuite *suite();
};

}

#endif // STEADYSTATETEST_HH
