#ifndef __FLUC_TEST_MERSENNEWISTERTEST_HH__
#define __FLUC_TEST_MERSENNEWISTERTEST_HH__

#include "unittest.hh"


namespace iNA {

/**
 * Simple test case, where I test two identical RNG against each other.
 *
 * Does not tell anything about the quality but allows to find basic bugs.
 */
class MersenneTwisterTest : public UnitTest::TestCase
{
public:

  virtual ~MersenneTwisterTest(){};

  /**
   * Direct comparison.
   */
  void compareRNG();

  /**
   * Tests the cummulative distribution.
   */
  void testCummulative();


public:
  /**
   * Constructs the test suite.
   */
  static UnitTest::TestSuite *suite();
};

}

#endif // MERSENNEWISTERTEST_HH
