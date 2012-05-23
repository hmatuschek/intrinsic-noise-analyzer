#ifndef __TEST_BENCHMARK_HH__
#define __TEST_BENCHMARK_HH__

#include "unittest.hh"
#include "models/linearnoiseapproximation.hh"

namespace Fluc {

class Benchmark : public UnitTest::TestCase
{
protected:
  Models::LinearNoiseApproximation *lna;

  void integrate_BCI_LSODA(Models::LinearNoiseApproximation *model,
                           double t, size_t opt_level);

  void integrate_BCIMP_LSODA(Models::LinearNoiseApproximation *model,
                             double t, size_t opt_level);

  void integrate_JIT_LSODA(Models::LinearNoiseApproximation *model,
                           double t, size_t opt_level);

  void integrate_BCI_Rosen4(Models::LinearNoiseApproximation *model,
                           double t, size_t opt_level);

  void integrate_BCIMP_Rosen4(Models::LinearNoiseApproximation *model,
                             double t, size_t opt_level);

  void integrate_JIT_Rosen4(Models::LinearNoiseApproximation *model,
                           double t, size_t opt_level);

public:
  void setUp();
  void tearDown();

  void testCoremodelBCILSODAOpt();
  void testCoremodelBCIMPLSODAOpt();
  void testCoremodelJITLSODAOpt();

  void testCoremodelBCILSODANoOpt();
  void testCoremodelBCIMPLSODANoOpt();
  void testCoremodelJITLSODANoOpt();

  void testCoremodelBCIRosen4Opt();
  void testCoremodelBCIMPRosen4Opt();
  void testCoremodelJITRosen4Opt();

  void testCoremodelBCIRosen4NoOpt();
  void testCoremodelBCIMPRosen4NoOpt();
  void testCoremodelJITRosen4NoOpt();

public:
  static UnitTest::TestSuite *suite();
};

}


#endif // BENCHMARK_HH
