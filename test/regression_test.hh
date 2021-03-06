#ifndef __FLUC_REGRESSION_TEST_HH__
#define __FLUC_REGRESSION_TEST_HH__

#include "unittest.hh"


namespace iNA {


class RegressionTest : public UnitTest::TestCase
{
public:
  void testConstSpecies();
  void testNonConstantParameter();
  void testNonConstantCompartment();
  void testNoAlgebraicConstraint();
  void testSpeciesByRateRule();

public:
  static UnitTest::TestSuite *suite();

  virtual ~RegressionTest();

};


}


#endif
