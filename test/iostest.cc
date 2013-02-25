#include "iostest.hh"
#include <parser/sbml/sbml.hh>
#include <models/sseinterpreter.hh>
#include <ode/lsodadriver.hh>

using namespace iNA;

// Define JIT engines:
typedef Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd> JITVectorEngine;
typedef Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> JITMatrixEngine;
// Define IOS JIT engine:
typedef Models::GenericSSEinterpreter<Models::IOSmodel, JITVectorEngine, JITMatrixEngine> IOSJITinterpreter;


IOSTest::~IOSTest() {
  // Pass...
}


void
IOSTest::testEnzymeKineticsBCI()
{
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");
  // Construct IOS model to integrate
  Models::IOSmodel model(sbml_model);
  // Perform integration:
  integrateViaByteCode(model);
}

void
IOSTest::testEnzymeKineticsJIT()
{
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");
  // Construct IOS model to integrate
  Models::IOSmodel model(sbml_model);
  // Perform integration:
  integrateViaJIT(model);
}


UnitTest::TestSuite *
IOSTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("IOS Tests");

  s->addTest(new UnitTest::TestCaller<IOSTest>(
               "EnzymeKinetics Model (BCI)", &IOSTest::testEnzymeKineticsBCI));

  s->addTest(new UnitTest::TestCaller<IOSTest>(
               "EnzymeKinetics Model (JIT)", &IOSTest::testEnzymeKineticsJIT));

  return s;
}


void
IOSTest::integrateViaByteCode(Models::IOSmodel &model)
{
  double err_abs = 1e-5;
  double err_rel = 1e-6;
  double final_time = 1.0;
  size_t N = 100;
  double dt=final_time/N;

  // Create interpreter & integrator w/o optimization:
  Models::IOSinterpreter interpreter(model, 0);
  ODE::LsodaDriver<Models::IOSinterpreter> stepper(interpreter, dt, err_abs, err_rel);

  // Allocate & initialize state vectors:
  Eigen::VectorXd x(model.getDimension()); model.getInitialState(x);
  Eigen::VectorXd dx(model.getDimension());

  // Perform integration:
  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt) {
    stepper.step(x,t,dx);
    x += dx; t += dt;
  }
}


void
IOSTest::integrateViaJIT(Models::IOSmodel &model)
{
  double err_abs = 1e-5;
  double err_rel = 1e-6;
  double final_time = 1.0;
  size_t N = 100;
  double dt=final_time/N;

  // Create interpreter & integrator w/o optimization:
  IOSJITinterpreter interpreter(model, 0);
  ODE::LsodaDriver<IOSJITinterpreter> stepper(interpreter, dt, err_abs, err_rel);

  // Allocate & initialize state vectors:
  Eigen::VectorXd x(model.getDimension()); model.getInitialState(x);
  Eigen::VectorXd dx(model.getDimension());

  // Perform integration:
  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt) {
    stepper.step(x,t,dx);
    x += dx; t += dt;
  }
}
