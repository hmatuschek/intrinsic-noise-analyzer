#ifndef __TEST_BENCHMARK__PSCAN_HH__
#define __TEST_BENCHMARK_PSCAN_HH__

#include "unittest.hh"
#include "models/IOSmodel.hh"
#include "utils/cputime.hh"

namespace iNA {

class BenchmarkPscan : public UnitTest::TestCase
{
protected:
  Ast::Model *_sbml_model;

  /** Generic benchmark for parameter scan. */
  template<class Model, class Analysis>
  void generic_solve(Model &model, double &cpu_time, double &real_time) {
    // Assemble param scan
    Analysis scan(model, Nstep, 1e-9);

    // Assemble parameter set:
    std::vector< std::map<std::string, double> > parameter_sets(Nstep);
    std::vector< Eigen::VectorXd > result_set(Nstep);
    double dp = (p_end - p_start)/Nstep; double p_value = p_start;
    for (size_t i=0; i<Nstep; i++, p_value += dp) {
      parameter_sets[i]["feedback"] = p_value;
      result_set[i].resize(model.getDimension());
    }

    // Perform actual benchmark
    Utils::CpuTime  cpu_clock;
    Utils::RealTime real_clock;
    cpu_clock.start(); real_clock.start();
    scan.parameterScan(parameter_sets, result_set, 1);
    cpu_time = cpu_clock.stop();
    real_time = real_clock.stop();
  }

  void solve_GiNaC_re(Models::REmodel &model);
  void solve_GiNaC_lna(Models::LNAmodel &model);
  void solve_GiNaC_ios(Models::IOSmodel &model);

  void solve_BCI_re(Models::REmodel &model);
  void solve_BCI_lna(Models::LNAmodel &model);
  void solve_BCI_ios(Models::IOSmodel &model);

  void solve_JIT_re(Models::REmodel &model);
  void solve_JIT_lna(Models::LNAmodel &model);
  void solve_JIT_ios(Models::IOSmodel &model);


protected:
  static double p_start;
  static double p_end;
  static size_t ensemble_size;
  static size_t Nstep;
  static double transient_time;
  static double ssa_time;

public:
  void setUp();
  void tearDown();

  void testCoremodelGiNaC();
  void testCoremodelBCI();
  void testCoremodelJIT();
  void testCoremodelSSA();

public:
  static UnitTest::TestSuite *suite();
  virtual ~BenchmarkPscan();
};

}


#endif // __INA_TEST_BENCHMARK_PSCAN_HH__
