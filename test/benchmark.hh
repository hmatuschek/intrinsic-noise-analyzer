#ifndef __TEST_BENCHMARK_HH__
#define __TEST_BENCHMARK_HH__

#include "unittest.hh"
#include "models/linearnoiseapproximation.hh"

namespace Fluc {

class Benchmark : public UnitTest::TestCase
{
protected:
  Models::LinearNoiseApproximation *lna;

public:
  void setUp();
  void tearDown();

  void testCoremodelBCILSODAOpt();
  void testCoremodelBCIMPLSODAOpt();
  void testCoremodelJITLSODAOpt();

  void testCoremodelBCILSODANoOpt();
  void testCoremodelBCIMPLSODANoOpt();
  void testCoremodelJITLSODANoOpt();

public:
  static UnitTest::TestSuite *suite();
};

}


#endif // BENCHMARK_HH
