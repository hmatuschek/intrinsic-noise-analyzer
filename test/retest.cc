#include "retest.hh"
#include <models/REmodel.hh>
#include <models/sseinterpreter.hh>
#include <parser/sbml/sbml.hh>
#include <ode/lsodadriver.hh>


using namespace iNA;

// Define JIT engines:
typedef Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd> JITVectorEngine;
typedef Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> JITMatrixEngine;
// Define IOS JIT engine:
typedef Models::GenericSSEinterpreter<Models::REmodel, JITVectorEngine, JITMatrixEngine> REJITinterpreter;


void
RETest::testEnzymeKineticsBCI()
{
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");
  // Construct IOS model to integrate
  Models::REmodel model(sbml_model);
  // Perform integration:
  integrateViaByteCode(model);
}

void
RETest::testEnzymeKineticsJIT()
{
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");
  // Construct IOS model to integrate
  Models::REmodel model(sbml_model);
  // Perform integration:
  integrateViaJIT(model);
}


UnitTest::TestSuite *
RETest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("RE Tests");

  s->addTest(new UnitTest::TestCaller<RETest>(
               "EnzymeKinetics Model (BCI)", &RETest::testEnzymeKineticsBCI));

  s->addTest(new UnitTest::TestCaller<RETest>(
               "EnzymeKinetics Model (JIT)", &RETest::testEnzymeKineticsJIT));

  return s;
}


void
RETest::integrateViaByteCode(Models::REmodel &model)
{
  double err_abs = 1e-5;
  double err_rel = 1e-6;
  double final_time = 1.0;
  size_t N = 100;
  double dt=final_time/N;

  // Create interpreter & integrator w/o optimization:
  Models::REinterpreter interpreter(model, 0);
  ODE::LsodaDriver<Models::REinterpreter> stepper(interpreter, dt, err_abs, err_rel);

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
RETest::integrateViaJIT(Models::REmodel &model)
{
  double err_abs = 1e-5;
  double err_rel = 1e-6;
  double final_time = 1.0;
  size_t N = 100;
  double dt=final_time/N;

  // Create interpreter & integrator w/o optimization:
  REJITinterpreter interpreter(model, 0);
  ODE::LsodaDriver<REJITinterpreter> stepper(interpreter, dt, err_abs, err_rel);

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

