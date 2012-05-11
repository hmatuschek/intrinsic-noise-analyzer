#include "lnatest.hh"
#include "exception.hh"
#include "utils/cputime.hh"
#include "models/lnainterpreter.hh"
#include "models/lnaevaluator.hh"
#include "ode/ode.hh"

using namespace Fluc;



void
LNATest::testEnzymeKineticsOpen()
{
  this->compareIntegrators("doc/sbmlmodels/enzymekinetics_open.xml", 1.0);
}

void
LNATest::testDimerization()
{
  this->compareIntegrators("doc/sbmlmodels/dimerization.xml", 0.10);
}

void
LNATest::testDimerization2()
{
  this->compareIntegrators("doc/sbmlmodels/dimerization2.xml", 1.0);
}


void
LNATest::compareIntegrators(const std::string &file, double final_time)
{
  double err_abs = 1e-5;
  double err_rel = 1e-6;

  // Read doc and check for errors:
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile(file.c_str());

  if (0 < doc->getNumErrors())
  {
    std::cerr << "Error: ";
    doc->printErrors(std::cerr); std::cerr << std::endl;
  }
  UT_ASSERT_EQUAL(doc->getNumErrors(), 0u);

  // Construct LNA model to integrate
  Models::LinearNoiseApproximation model(doc->getModel());

  Eigen::VectorXd initial_state(model.getDimension());
  Eigen::VectorXd final_state_ginac(model.getDimension());
  Eigen::VectorXd final_state_intpr(model.getDimension());

  model.getInitialState(initial_state);

  this->integrateViaGiNaC(model, initial_state, final_state_ginac, final_time, err_abs, err_rel);
  this->integrateViaByteCode(model, initial_state, final_state_intpr, final_time, err_abs, err_rel);

  for (size_t i=0; i<model.getDimension(); i++)
  {
    UT_ASSERT(abs(final_state_intpr(i) - final_state_ginac(i))
              < err_rel*abs(final_state_ginac(i)) + err_abs);
  }

  delete doc;
}



void
LNATest::integrateViaGiNaC(Models::LinearNoiseApproximation &model,
                           const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                           double final_time, double err_abs, double err_rel)
{
  size_t N = 100;
  double dt=final_time/N;

  Models::LNAevaluator evaluator(model);
  ODE::RKF45<Models::LNAevaluator> integrator(evaluator, dt, err_abs, err_rel);

  // state vector (deterministic concentrations + covariances)
  Eigen::VectorXd x(init_state);
  Eigen::VectorXd dx(integrator.getDimension());

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  final_state.noalias() = x;
}


void
LNATest::integrateViaByteCode(Models::LinearNoiseApproximation &model,
                              const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                              double final_time, double err_abs, double err_rel)
{
  size_t N = 100;
  double dt=final_time/N;

  Models::LNAinterpreter interpreter(model, 1);
  ODE::RKF45<Models::LNAinterpreter> integrator(interpreter, dt, err_abs, err_rel);

  // state vector (deterministic concentrations + covariances)
  Eigen::VectorXd x(init_state);
  Eigen::VectorXd dx(integrator.getDimension());

  double t = 0.0;
  for(size_t i=0; i<N; i++,t+=dt)
  {
    integrator.step(x,t,dx);
    x += dx; t += dt;
  }

  final_state.noalias() = x;
}




UnitTest::TestSuite *
LNATest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Linear Noise Approximation Tests");

  s->addTest(new UnitTest::TestCaller<LNATest>(
               "EnzymeKineticsOpen.", &LNATest::testEnzymeKineticsOpen));

  /*s->addTest(new UnitTest::TestCaller<LNATest>(
               "Dimerization.", &LNATest::testDimerization));*/

  s->addTest(new UnitTest::TestCaller<LNATest>(
               "Dimerization 2.", &LNATest::testDimerization2));

  return s;
}
