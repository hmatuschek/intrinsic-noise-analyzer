#include "benchmark.hh"
#include "models/lnainterpreter.hh"
#include "models/gillespieSSA.hh"
#include "models/optimizedSSA.hh"

#include "ode/rosenbrock4.hh"
#include "ode/lsodadriver.hh"

#include "eval/bci/engine.hh"
#include "eval/bcimp/engine.hh"
#include "eval/llvm/engine.hh"
#include "eval/direct/engine.hh"

#include "utils/cputime.hh"


using namespace Fluc;

typedef Models::GenericLNAinterpreter< Evaluate::bci::Engine<Eigen::VectorXd>, Evaluate::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > BCIInterpreter;
typedef Models::GenericLNAinterpreter< Evaluate::bcimp::Engine<Eigen::VectorXd>, Evaluate::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > BCIMPInterpreter;
typedef Models::GenericLNAinterpreter< Evaluate::LLVM::Engine<Eigen::VectorXd>, Evaluate::LLVM::Engine<Eigen::VectorXd, Eigen::MatrixXd> > LLVMInterpreter;
typedef Models::GenericLNAinterpreter< Evaluate::direct::Engine<Eigen::VectorXd>, Evaluate::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > GiNaCInterpreter;

typedef Models::GenericGillespieSSA< Evaluate::bci::Engine<Eigen::VectorXd> > GillespieBCI;
typedef Models::GenericGillespieSSA< Evaluate::LLVM::Engine<Eigen::VectorXd> > GillespieJIT;
typedef Models::GenericGillespieSSA< Evaluate::direct::Engine<Eigen::VectorXd> > GillespieGiNaC;

typedef Models::GenericOptimizedSSA< Evaluate::bci::Engine<Eigen::VectorXd> > OptSSABCI;
typedef Models::GenericOptimizedSSA< Evaluate::LLVM::Engine<Eigen::VectorXd> > OptSSAJIT;
typedef Models::GenericOptimizedSSA< Evaluate::direct::Engine<Eigen::VectorXd> > OptSSAGiNaC;


size_t Benchmark::N_steps = 100;
double Benchmark::eps_abs = 1e-10;
double Benchmark::eps_rel = 1e-6;
double Benchmark::t_end   = 5.0;
size_t Benchmark::ensemble_size = 300; // 3000

void
Benchmark::setUp()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/coremodel2.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  this->lna = new Models::LinearNoiseApproximation(doc->getModel());
  this->document = doc;
}


void
Benchmark::tearDown()
{
  delete this->lna;
  delete this->document;
}


void
Benchmark::integrate_BCI_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  BCIInterpreter interpreter(*model, opt_level);
  ODE::LsodaDriver<BCIInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (BCI): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}

void
Benchmark::integrate_BCIMP_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  BCIMPInterpreter interpreter(*model, opt_level);
  ODE::LsodaDriver<BCIMPInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (BCIMP): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}

void
Benchmark::integrate_JIT_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  LLVMInterpreter interpreter(*model, opt_level);
  ODE::LsodaDriver<LLVMInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  integrator.step(x, 0.0, dx);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (JIT): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::integrate_GiNaC_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  GiNaCInterpreter interpreter(*model, opt_level);
  ODE::LsodaDriver<GiNaCInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  integrator.step(x, 0.0, dx);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (GiNaC): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::integrate_BCI_Rosen4(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  BCIInterpreter interpreter(*model, opt_level);
  ODE::Rosenbrock4TimeInd<BCIInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (BCI): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}

void
Benchmark::integrate_BCIMP_Rosen4(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  BCIMPInterpreter interpreter(*model, opt_level);
  ODE::Rosenbrock4TimeInd<BCIMPInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (BCIMP): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}

void
Benchmark::integrate_JIT_Rosen4(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  double dt = t_end/N_steps;

  LLVMInterpreter interpreter(*model, opt_level);
  ODE::Rosenbrock4TimeInd<LLVMInterpreter> integrator(interpreter, dt, eps_abs, eps_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  // Force code-emmission:
  integrator.step(x, 0.0, dx);

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  double t = 0.0;
  for(size_t i=0; i<N_steps; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  std::cout << "Precise execution time (JIT): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::simulate_BCI_gillespie(libsbml::Model *model, double t, size_t opt_level)
{
  GillespieBCI simulator(model, ensemble_size, 1234, opt_level, OpenMP::getMaxThreads());
  double dt=t/N_steps;

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  for (size_t i=0; i<N_steps; i++) {
    simulator.run(dt);
  }

  std::cout << "Precise execution time (BCI): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}

void
Benchmark::simulate_JIT_gillespie(libsbml::Model *model, double t, size_t opt_level)
{
  GillespieJIT simulator(model, ensemble_size, 1234, opt_level, OpenMP::getMaxThreads());
  double dt=t/N_steps;

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  for (size_t i=0; i<N_steps; i++) {
    simulator.run(dt);
  }

  std::cout << "Precise execution time (JIT): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::simulate_GiNaC_gillespie(libsbml::Model *model, double t, size_t opt_level)
{
  GillespieGiNaC simulator(model, ensemble_size, 1234, opt_level, 1);
  double dt=t/N_steps;

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  for (size_t i=0; i<N_steps; i++) {
    simulator.run(dt);
  }

  std::cout << "Precise execution time (GiNaC): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::simulate_BCI_optSSA(libsbml::Model *model, double t, size_t opt_level)
{
  OptSSABCI simulator(model, ensemble_size, 1234, opt_level, OpenMP::getMaxThreads());
  double dt=t/N_steps;

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  for (size_t i=0; i<N_steps; i++) {
    simulator.run(dt);
  }

  std::cout << "Precise execution time (BCI): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::simulate_JIT_optSSA(libsbml::Model *model, double t, size_t opt_level)
{
  OptSSAJIT simulator(model, ensemble_size, 1234, opt_level, OpenMP::getMaxThreads());
  double dt=t/N_steps;

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  for (size_t i=0; i<N_steps; i++) {
    simulator.run(dt);
  }

  std::cout << "Precise execution time (JIT): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::simulate_GiNaC_optSSA(libsbml::Model *model, double t, size_t opt_level)
{
  OptSSAGiNaC simulator(model, ensemble_size, 1234, opt_level, 1);
  double dt=t/N_steps;

  Utils::CpuTime  cpu_clock; cpu_clock.start();
  Utils::RealTime real_clock; real_clock.start();

  for (size_t i=0; i<N_steps; i++) {
    simulator.run(dt);
  }

  std::cout << "Precise execution time (GiNaC): " << std::endl
            << "  cpu: " << cpu_clock.stop() << "s." << std::endl
            << " real: " << real_clock.stop() << "s." << std::endl;
}


void
Benchmark::testCoremodelBCILSODAOpt()
{
  integrate_BCI_LSODA(this->lna, t_end, 1);
}


void
Benchmark::testCoremodelBCIMPLSODAOpt()
{
  integrate_BCIMP_LSODA(this->lna, t_end, 1);
}


void
Benchmark::testCoremodelJITLSODAOpt()
{
  integrate_JIT_LSODA(this->lna, t_end, 1);
}


void
Benchmark::testCoremodelGiNaCLSODA()
{
  integrate_GiNaC_LSODA(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelBCILSODANoOpt()
{
  integrate_BCI_LSODA(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelBCIMPLSODANoOpt()
{
  integrate_BCIMP_LSODA(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelJITLSODANoOpt()
{
  integrate_JIT_LSODA(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelBCIRosen4Opt()
{
  integrate_BCI_Rosen4(this->lna, t_end, 1);
}


void
Benchmark::testCoremodelBCIMPRosen4Opt()
{
  integrate_BCIMP_Rosen4(this->lna, t_end, 1);
}


void
Benchmark::testCoremodelJITRosen4Opt()
{
  integrate_JIT_Rosen4(this->lna, t_end, 1);
}


void
Benchmark::testCoremodelBCIRosen4NoOpt()
{
  integrate_BCI_Rosen4(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelBCIMPRosen4NoOpt()
{
  integrate_BCIMP_Rosen4(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelJITRosen4NoOpt()
{
  integrate_JIT_Rosen4(this->lna, t_end, 0);
}


void
Benchmark::testCoremodelBCIGillespieOpt()
{
  simulate_BCI_gillespie(this->document->getModel(), t_end, 1);
}

void
Benchmark::testCoremodelBCIGillespieNoOpt()
{
  simulate_BCI_gillespie(this->document->getModel(), t_end, 0);
}

void
Benchmark::testCoremodelJITGillespieOpt()
{
  simulate_JIT_gillespie(this->document->getModel(), t_end, 1);
}

void
Benchmark::testCoremodelJITGillespieNoOpt()
{
  simulate_JIT_gillespie(this->document->getModel(), t_end, 0);
}

void
Benchmark::testCoremodelGiNaCGillespie()
{
  simulate_GiNaC_gillespie(this->document->getModel(), t_end, 0);
}

void
Benchmark::testCoremodelBCIOptSSAOpt()
{
  simulate_BCI_optSSA(this->document->getModel(), t_end, 1);
}

void
Benchmark::testCoremodelBCIOptSSANoOpt()
{
  simulate_BCI_optSSA(this->document->getModel(), t_end, 0);
}

void
Benchmark::testCoremodelJITOptSSAOpt()
{
  simulate_JIT_optSSA(this->document->getModel(), t_end, 1);
}

void
Benchmark::testCoremodelJITOptSSANoOpt()
{
  simulate_JIT_optSSA(this->document->getModel(), t_end, 0);
}

void
Benchmark::testCoremodelGiNaCOptSSA()
{
  simulate_GiNaC_optSSA(this->document->getModel(), t_end, 0);
}


UnitTest::TestSuite *
Benchmark::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Engine benchmark");

  /*s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (OptSSA, JIT, Opt)", &Benchmark::testCoremodelJITOptSSAOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (OptSSA, JIT)", &Benchmark::testCoremodelJITOptSSANoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (OptSSA, BCI, Opt)", &Benchmark::testCoremodelBCIOptSSAOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (OptSSA, BCI)", &Benchmark::testCoremodelBCIOptSSANoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (OptSSA, GiNaC)", &Benchmark::testCoremodelGiNaCOptSSA));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, BCI, Opt)", &Benchmark::testCoremodelBCILSODAOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, BCIMP, Opt)", &Benchmark::testCoremodelBCIMPLSODAOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, LLVM, Opt)", &Benchmark::testCoremodelJITLSODAOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, BCI)", &Benchmark::testCoremodelBCILSODANoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, BCIMP)", &Benchmark::testCoremodelBCIMPLSODANoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, LLVM)", &Benchmark::testCoremodelJITLSODANoOpt)); */

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCI)", &Benchmark::testCoremodelBCIRosen4NoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCI, Opt)", &Benchmark::testCoremodelBCIRosen4Opt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, LLVM)", &Benchmark::testCoremodelJITRosen4NoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, LLVM, Opt)", &Benchmark::testCoremodelJITRosen4Opt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCIMP, Opt)", &Benchmark::testCoremodelBCIMPRosen4Opt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCIMP)", &Benchmark::testCoremodelBCIMPRosen4NoOpt));

  /*s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Gillespie, BCI, Opt)", &Benchmark::testCoremodelBCIGillespieOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Gillespie, BCI)", &Benchmark::testCoremodelBCIGillespieNoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Gillespie, JIT, Opt)", &Benchmark::testCoremodelJITGillespieOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Gillespie, JIT)", &Benchmark::testCoremodelJITGillespieNoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Gillespie, GiNaC)", &Benchmark::testCoremodelGiNaCGillespie));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (LSODA, GiNaC)", &Benchmark::testCoremodelGiNaCLSODA));*/


  return s;
}
