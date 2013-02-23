#include "iostest.hh"
#include <parser/sbml/sbml.hh>
#include <models/sseinterpreter.hh>
#include <ode/lsodadriver.hh>

using namespace iNA;


IOSTest::~IOSTest() {
  // Pass...
}


void
IOSTest::testCoreModel()
{
  double err_abs = 1e-5;
  double err_rel = 1e-6;
  double final_time = 1.0;

  // Read doc and check for errors:
  Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, "test/regression-tests/coremodel1.xml");
  // Construct LNA model to integrate
  Models::IOSmodel model(sbml_model);
  // Allocate states:
  Eigen::VectorXd initial_state(model.getDimension());
  Eigen::VectorXd final_state_intpr(model.getDimension());
  // Get initial state:
  model.getInitialState(initial_state);
  // Perform integration:
  integrateViaByteCode(model, initial_state, final_state_intpr, final_time, err_abs, err_rel);
}


UnitTest::TestSuite *
IOSTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("IOS Tests");

  s->addTest(new UnitTest::TestCaller<IOSTest>(
               "Core Model 1", &IOSTest::testCoreModel));

  return s;
}


void
IOSTest::integrateViaByteCode(Models::IOSmodel &model,
                              const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                              double final_time, double err_abs, double err_rel)
{
  size_t N = 100;
  double dt=final_time/N;

  // Create interpreter & integrator w/o optimization:
  Models::IOSinterpreter interpreter(model, 0);
  ODE::LsodaDriver<Models::IOSinterpreter> stepper(interpreter, dt, err_abs, err_rel);

  // Allocate & initialize state vectors:
  Eigen::VectorXd x(init_state);
  Eigen::VectorXd dx(interpreter.getDimension());

  // Perform integration:
  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt) {
    stepper.step(x,t,dx);
    x += dx; t += dt;
  }
  final_state.noalias() = x;
}

