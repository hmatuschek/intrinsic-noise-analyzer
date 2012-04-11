#ifndef __FLUC_REGRESSION_TEST_HH__
#define __FLUC_REGRESSION_TEST_HH__

#include "unittest.hh"


namespace Fluc {


class RegressionTest : public UnitTest::TestCase
{
public:
  void testConstSpecies();
  void testSpeciesByAssignmentRule();
  void testNonConstantParameter();
  void testNonConstantCompartment();
  void testAlgebraicConstraint();
  void testSpeciesByRateRule();

public:
  static UnitTest::TestSuite *suite();
};


}


#endif
