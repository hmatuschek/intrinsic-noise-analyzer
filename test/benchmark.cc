#include "benchmark.hh"
#include "models/lnainterpreter.hh"

#include "ode/rosenbrock4.hh"
#include "ode/lsodadriver.hh"

#include "eval/bci/engine.hh"
#include "eval/bcimp/engine.hh"
#include "eval/llvm/engine.hh"

using namespace Fluc;

typedef Models::GenericLNAinterpreter< Evaluate::bci::Engine<Eigen::VectorXd>, Evaluate::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > BCIInterpreter;
typedef Models::GenericLNAinterpreter< Evaluate::bcimp::Engine<Eigen::VectorXd>, Evaluate::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > BCIMPInterpreter;
typedef Models::GenericLNAinterpreter< Evaluate::LLVM::Engine<Eigen::VectorXd>, Evaluate::LLVM::Engine<Eigen::VectorXd, Eigen::MatrixXd> > LLVMInterpreter;


void
Benchmark::setUp()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/coremodel1.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  this->lna = new Models::LinearNoiseApproximation(doc->getModel());

  delete doc;
}


void
Benchmark::tearDown()
{
  delete this->lna;
}


void
Benchmark::integrate_BCI_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  BCIInterpreter interpreter(*model, opt_level);

  size_t N = 100;
  double dt = t_end/N;
  double err_abs = 1e-6;
  double err_rel = 1e-4;

  ODE::LsodaDriver<BCIInterpreter> integrator(interpreter, dt, err_abs, err_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }
}

void
Benchmark::integrate_BCIMP_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  BCIMPInterpreter interpreter(*model, opt_level);

  size_t N = 100;
  double dt = t_end/N;
  double err_abs = 1e-6;
  double err_rel = 1e-4;

  ODE::LsodaDriver<BCIMPInterpreter> integrator(interpreter, dt, err_abs, err_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }
}

void
Benchmark::integrate_JIT_LSODA(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  LLVMInterpreter interpreter(*model, opt_level);

  size_t N = 100;
  double dt = t_end/N;
  double err_abs = 1e-6;
  double err_rel = 1e-4;

  ODE::LsodaDriver<LLVMInterpreter> integrator(interpreter, dt, err_abs, err_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }
}

void
Benchmark::integrate_BCI_Rosen4(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  BCIInterpreter interpreter(*model, opt_level);

  size_t N = 100;
  double dt = t_end/N;
  double err_abs = 1e-6;
  double err_rel = 1e-4;

  ODE::Rosenbrock4TimeInd<BCIInterpreter> integrator(interpreter, dt, err_abs, err_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }
}

void
Benchmark::integrate_BCIMP_Rosen4(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  BCIMPInterpreter interpreter(*model, opt_level);

  size_t N = 100;
  double dt = t_end/N;
  double err_abs = 1e-6;
  double err_rel = 1e-4;

  ODE::Rosenbrock4TimeInd<BCIMPInterpreter> integrator(interpreter, dt, err_abs, err_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }
}

void
Benchmark::integrate_JIT_Rosen4(Models::LinearNoiseApproximation *model, double t_end, size_t opt_level)
{
  LLVMInterpreter interpreter(*model, opt_level);

  size_t N = 100;
  double dt = t_end/N;
  double err_abs = 1e-6;
  double err_rel = 1e-4;

  ODE::Rosenbrock4TimeInd<LLVMInterpreter> integrator(interpreter, dt, err_abs, err_rel);

  Eigen::VectorXd x(interpreter.getDimension());
  Eigen::VectorXd dx(interpreter.getDimension());
  lna->getInitialState(x);

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }
}


void
Benchmark::testCoremodelBCILSODAOpt()
{
  integrate_BCI_LSODA(this->lna, 1, 1);
}


void
Benchmark::testCoremodelBCIMPLSODAOpt()
{
  integrate_BCIMP_LSODA(this->lna, 1, 1);
}


void
Benchmark::testCoremodelJITLSODAOpt()
{
  integrate_JIT_LSODA(this->lna, 1, 1);
}


void
Benchmark::testCoremodelBCILSODANoOpt()
{
  integrate_BCI_LSODA(this->lna, 1., 0);
}


void
Benchmark::testCoremodelBCIMPLSODANoOpt()
{
  integrate_BCIMP_LSODA(this->lna, 1., 0);
}


void
Benchmark::testCoremodelJITLSODANoOpt()
{
  integrate_JIT_LSODA(this->lna, 1., 0);
}


void
Benchmark::testCoremodelBCIRosen4Opt()
{
  integrate_BCI_Rosen4(this->lna, 1, 1);
}


void
Benchmark::testCoremodelBCIMPRosen4Opt()
{
  integrate_BCIMP_Rosen4(this->lna, 1, 1);
}


void
Benchmark::testCoremodelJITRosen4Opt()
{
  integrate_JIT_Rosen4(this->lna, 1, 1);
}


void
Benchmark::testCoremodelBCIRosen4NoOpt()
{
  integrate_BCI_Rosen4(this->lna, 1., 0);
}


void
Benchmark::testCoremodelBCIMPRosen4NoOpt()
{
  integrate_BCIMP_Rosen4(this->lna, 1., 0);
}


void
Benchmark::testCoremodelJITRosen4NoOpt()
{
  integrate_JIT_Rosen4(this->lna, 1., 0);
}


UnitTest::TestSuite *
Benchmark::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Engine benchmark");

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
               "Coremodel 1 (LSODA, LLVM)", &Benchmark::testCoremodelJITLSODANoOpt));


  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCI, Opt)", &Benchmark::testCoremodelBCIRosen4Opt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCIMP, Opt)", &Benchmark::testCoremodelBCIMPRosen4Opt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, LLVM, Opt)", &Benchmark::testCoremodelJITRosen4Opt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCI)", &Benchmark::testCoremodelBCIRosen4NoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, BCIMP)", &Benchmark::testCoremodelBCIMPRosen4NoOpt));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen4, LLVM)", &Benchmark::testCoremodelJITRosen4NoOpt));
  return s;
}
