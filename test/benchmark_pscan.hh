#ifndef __TEST_BENCHMARK__PSCAN_HH__
#define __TEST_BENCHMARK_PSCAN_HH__

#include "unittest.hh"
#include "models/IOSmodel.hh"

namespace iNA {

class BenchmarkPscan : public UnitTest::TestCase
{
protected:
  Ast::Model *_sbml_model;

  void solve_GiNaC_re(Models::REmodel *model);
  void solve_GiNaC_lna(Ast::Model *model);
  void solve_GiNaC_ios(Ast::Model *model);

  void solve_BCI_re(Ast::Model *model);
  void solve_BCI_lna(Ast::Model *model);
  void solve_BCI_ios(Ast::Model *model);

  void solve_JIT_re(Ast::Model *model);
  void solve_JIT_lna(Ast::Model *model);
  void solve_JIT_ios(Ast::Model *model);

protected:
  static double p_start;
  static double p_end;
  static size_t ensemble_size;

public:
  void setUp();
  void tearDown();

  void testCoremodelGiNaC();

public:
  static UnitTest::TestSuite *suite();

  virtual ~BenchmarkPscan();
};

}


#endif // __INA_TEST_BENCHMARK_PSCAN_HH__
