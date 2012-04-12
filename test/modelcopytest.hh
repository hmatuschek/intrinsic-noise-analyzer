#ifndef MODELCOPYTEST_HH
#define MODELCOPYTEST_HH

#include "unittest.hh"
#include "models/linearnoiseapproximation.hh"

namespace Fluc {

/**
 * Simple test to check if copying of models work.
 */
class ModelCopyTest : public UnitTest::TestCase
{
protected:
  // Performs the test:
  void testCopy(const std::string &file);


public:
  /** Tests copying on ./test/regression-tests/coopkinetics1.xml. */
  void testCoopKinetics1();


public:
  static UnitTest::TestSuite *suite();
};

}

#endif // MODELCOPYTEST_HH
