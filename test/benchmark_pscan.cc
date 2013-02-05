#include "benchmark_pscan.hh"

#include "models/REmodel.hh"
#include "models/LNAmodel.hh"
#include "models/IOSmodel.hh"
#include "models/sseparamscan.hh"

#include "eval/direct/engine.hh"
#include "eval/bci/engine.hh"
#include "eval/jit/engine.hh"

#include "utils/cputime.hh"

#include "parser/parser.hh"



using namespace iNA;

typedef Models::ParameterScan<Models::REmodel,
  Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_RE_GiNaC;
typedef Models::ParameterScan<Models::LNAmodel,
  Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_LNA_GiNaC;
typedef Models::ParameterScan<Models::IOSmodel,
  Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_IOS_GiNaC;

double BenchmarkPscan::p_start = 0.0;
double BenchmarkPscan::p_end   = 0.5;
size_t BenchmarkPscan::ensemble_size = 300; // 3000

BenchmarkPscan::~BenchmarkPscan()
{
 // pass
}

void
BenchmarkPscan::setUp()
{
  _sbml_model = Parser::Sbml::importModel("test/regression-tests/core_final.xml");
}


void
BenchmarkPscan::tearDown()
{
  delete _sbml_model;
}


void
BenchmarkPscan::solve_GiNaC_re(Models::REmodel *model)
{
  size_t N=100;
  // Assemble param scan
  SSE_PScan_RE_GiNaC scan_ginac(*model, N, 1e-9);

  // Assemble paramter set:
  std::vector< std::map<std::string, double> > parameter_sets(N);
  std::vector< Eigen::VectorXd > result_set(N);
  double dp = (p_end - p_start)/N; double p_value = p_start;
  for (size_t i=0; i<N; i++, p_value += dp) {
    parameter_sets[i]["fac"] = p_value;
    result_set.resize(model->getDimension());
  }

  Utils::CpuTime  cpu_clock;
  Utils::RealTime real_clock;

  cpu_clock.start(); real_clock.start();
  scan_ginac.parameterScan(parameter_sets, result_set, 1);
  std::cout << "Precise execution time (SSE, RE, GiNaC): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
BenchmarkPscan::testCoremodelGiNaC()
{
  Models::REmodel re_model(*_sbml_model);

  solve_GiNaC_re(&re_model);
}







UnitTest::TestSuite *
BenchmarkPscan::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Engine benchmark PScan");

  s->addTest(new UnitTest::TestCaller<BenchmarkPscan>(
               "Coremodel (SSE, GiNaC)", &BenchmarkPscan::testCoremodelGiNaC));

  return s;
}
