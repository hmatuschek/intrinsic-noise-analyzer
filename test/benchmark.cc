#include "benchmark.hh"
#include "models/lnainterpreter.hh"

#include "ode/rosenbrock3.hh"
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
Benchmark::testCoremodelBCI()
{
  BCIInterpreter interpreter(*this->lna, 1);

  size_t N = 100;
  double dt = 1.0/N;
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
Benchmark::testCoremodelBCIMP()
{
  BCIMPInterpreter interpreter(*this->lna, 1);

  size_t N = 100;
  double dt = 1.0/N;
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
Benchmark::testCoremodelJIT()
{
  LLVMInterpreter interpreter(*this->lna, 1);

  size_t N = 100;
  double dt = 1.0/N;
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


UnitTest::TestSuite *
Benchmark::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Engine benchmark");

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen3, BCI)", &Benchmark::testCoremodelBCI));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen3, BCIMP)", &Benchmark::testCoremodelBCIMP));

  s->addTest(new UnitTest::TestCaller<Benchmark>(
               "Coremodel 1 (Rosen3, LLVM)", &Benchmark::testCoremodelJIT));

  return s;
}
