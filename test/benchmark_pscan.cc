#include "benchmark_pscan.hh"

#include "models/REmodel.hh"
#include "models/LNAmodel.hh"
#include "models/IOSmodel.hh"
#include "models/sseparamscan.hh"
#include "models/ssaparamscan.hh"

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

typedef Models::ParameterScan<Models::REmodel,
  Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_RE_BCI;
typedef Models::ParameterScan<Models::LNAmodel,
  Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_LNA_BCI;
typedef Models::ParameterScan<Models::IOSmodel,
  Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_IOS_BCI;

typedef Models::ParameterScan<Models::REmodel,
  Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_RE_JIT;
typedef Models::ParameterScan<Models::LNAmodel,
  Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_LNA_JIT;
typedef Models::ParameterScan<Models::IOSmodel,
  Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
  Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > SSE_PScan_IOS_JIT;

typedef Models::SSAparamScan< Eval::direct::Engine<Eigen::VectorXd> > SSA_PSCan_GiNaC;
typedef Models::SSAparamScan< Eval::bci::Engine<Eigen::VectorXd> > SSA_PSCan_BCI;
typedef Models::SSAparamScan< Eval::jit::Engine<Eigen::VectorXd> > SSA_PSCan_JIT;


double BenchmarkPscan::p_start = 0.0;
double BenchmarkPscan::p_end   = 0.5;
size_t BenchmarkPscan::ensemble_size = 300; // 3000
size_t BenchmarkPscan::Nstep = 100;
double BenchmarkPscan::transient_time = 5.0;
double BenchmarkPscan::ssa_time = 100;


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
BenchmarkPscan::solve_GiNaC_re(Models::REmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::REmodel, SSE_PScan_RE_GiNaC>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, RE, GiNaC): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}

void
BenchmarkPscan::solve_GiNaC_lna(Models::LNAmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::LNAmodel, SSE_PScan_LNA_GiNaC>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, LNA, GiNaC): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}

void
BenchmarkPscan::solve_GiNaC_ios(Models::IOSmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::IOSmodel, SSE_PScan_IOS_GiNaC>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, IOS, GiNaC): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}


void
BenchmarkPscan::solve_BCI_re(Models::REmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::REmodel, SSE_PScan_RE_BCI>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, RE, BCI): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}

void
BenchmarkPscan::solve_BCI_lna(Models::LNAmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::LNAmodel, SSE_PScan_LNA_BCI>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, LNA, BCI): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}

void
BenchmarkPscan::solve_BCI_ios(Models::IOSmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::IOSmodel, SSE_PScan_IOS_BCI>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, IOS, BCI): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}


void
BenchmarkPscan::solve_JIT_re(Models::REmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::REmodel, SSE_PScan_RE_JIT>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, RE, JIT): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}

void
BenchmarkPscan::solve_JIT_lna(Models::LNAmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::LNAmodel, SSE_PScan_LNA_JIT>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, LNA, JIT): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}

void
BenchmarkPscan::solve_JIT_ios(Models::IOSmodel &model) {
  double cpu_time, real_time;
  generic_solve<Models::IOSmodel, SSE_PScan_IOS_JIT>(model, cpu_time, real_time);
  std::cout << "Precise execution time (SSE, IOS, JIT): " << std::endl
            << "  cpu: " << cpu_time << "s." << std::endl
            << " real: " << real_time << "s." << std::endl;
}


void
BenchmarkPscan::testCoremodelGiNaC()
{
  Models::REmodel  re_model(*_sbml_model);
  Models::LNAmodel lna_model(*_sbml_model);
  Models::IOSmodel ios_model(*_sbml_model);

  solve_GiNaC_re(re_model);
  solve_GiNaC_lna(lna_model);
  solve_GiNaC_ios(ios_model);
}


void
BenchmarkPscan::testCoremodelBCI()
{
  Models::REmodel  re_model(*_sbml_model);
  Models::LNAmodel lna_model(*_sbml_model);
  Models::IOSmodel ios_model(*_sbml_model);

  solve_BCI_re(re_model);
  solve_BCI_lna(lna_model);
  solve_BCI_ios(ios_model);
}


void
BenchmarkPscan::testCoremodelJIT()
{
  Models::REmodel  re_model(*_sbml_model);
  Models::LNAmodel lna_model(*_sbml_model);
  Models::IOSmodel ios_model(*_sbml_model);

  solve_JIT_re(re_model);
  solve_JIT_lna(lna_model);
  solve_JIT_ios(ios_model);
}


void
BenchmarkPscan::testCoremodelSSA()
{
  // Assemble parameter set:
  std::vector< std::map<std::string, double> > parameter_sets(Nstep);
  double dp = (p_end - p_start)/Nstep; double p_value = p_start;
  for (size_t i=0; i<Nstep; i++, p_value += dp) {
    parameter_sets[i]["feedback"] = p_value;
  }

  SSA_PSCan_GiNaC  pscan_ginac(*_sbml_model, parameter_sets, transient_time, 1, 0);
  SSA_PSCan_BCI    pscan_bci(*_sbml_model, parameter_sets, transient_time, 1, 0);
  SSA_PSCan_JIT    pscan_jit(*_sbml_model, parameter_sets, transient_time, 1, 0);

  Utils::CpuTime  cpu_clock;
  Utils::RealTime real_clock;
  double dt = ssa_time/Nstep;

  // Perform actual benchmark
  cpu_clock.start(); real_clock.start();
  for (size_t i=0; i<Nstep; i++) {
    pscan_ginac.run(dt);
  }
  std::cout << "Precise execution time (SSA, GiNaC): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;

  // Perform actual benchmark
  cpu_clock.start(); real_clock.start();
  for (size_t i=0; i<Nstep; i++) {
    pscan_bci.run(dt);
  }
  std::cout << "Precise execution time (SSA, BCI): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;

  // Perform actual benchmark
  cpu_clock.start(); real_clock.start();
  for (size_t i=0; i<Nstep; i++) {
    pscan_jit.run(dt);
  }
  std::cout << "Precise execution time (SSA, JIT): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}




UnitTest::TestSuite *
BenchmarkPscan::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Engine benchmark PScan");

  /*s->addTest(new UnitTest::TestCaller<BenchmarkPscan>(
               "Coremodel (SSE, GiNaC)", &BenchmarkPscan::testCoremodelGiNaC));

  s->addTest(new UnitTest::TestCaller<BenchmarkPscan>(
               "Coremodel (SSE, BCI)", &BenchmarkPscan::testCoremodelBCI));

  s->addTest(new UnitTest::TestCaller<BenchmarkPscan>(
               "Coremodel (SSE, JIT)", &BenchmarkPscan::testCoremodelJIT));*/

  s->addTest(new UnitTest::TestCaller<BenchmarkPscan>(
               "Coremodel (SSA)", &BenchmarkPscan::testCoremodelSSA));
  return s;
}
