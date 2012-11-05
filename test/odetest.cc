#include "odetest.hh"

#include "ode/ode.hh"


using namespace iNA;


ODETest::ODETest()
  : UnitTest::TestCase()
{
  { // Assemble harmonic oscillator system:
    GiNaC::symbol u("u"), v("v");
    std::vector<GiNaC::symbol> symbols(2); symbols[0]=u; symbols[1]=v;
    Eigen::VectorXex odes(2);

    odes(0) = v;
    odes(1) = -u;

    this->harm = new ODE::TimeIndepODEModel(symbols, odes, 1);
  }

  { // Assemble Stiff system stolen from "Numerical Recipes in C":
    GiNaC::symbol u("u"), v("v");
    std::vector<GiNaC::symbol> symbols(2); symbols[0]=u; symbols[1]=v;
    Eigen::VectorXex odes(2);

    odes(0) = 998*u + 1998*v;
    odes(1) = -999*u - 1999*v;

    this->stiff = new ODE::TimeIndepODEModel(symbols, odes, 1);
  }

  { // Assemble van der Pol system:
    GiNaC::symbol u("u"), v("v");
    std::vector<GiNaC::symbol> symbols(2); symbols[0]=u; symbols[1]=v;
    Eigen::VectorXex odes(2);
    double eta = 1000.0;

    odes(0) = v;
    odes(1) = eta*(1.-u*u)*v - u;

    this->vanDerPol = new ODE::TimeIndepODEModel(symbols, odes, 1);
  }
}

ODETest::~ODETest()
{
  // Pass...
}


void
ODETest::testStiffRKF45()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::RKF45<ODE::TimeIndepODEModel> stepper(*this->stiff, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<10; i++) {
    double x = 2*std::exp(-(i*dt)) - std::exp(-1000*(i*dt));
    double y = -std::exp(-(i*dt)) + std::exp(-1000*(i*dt));

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testStiffDopri5()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Dopri5Stepper<ODE::TimeIndepODEModel> stepper(*this->stiff, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<10; i++) {
    double x = 2*std::exp(-(i*dt)) - std::exp(-1000*(i*dt));
    double y = -std::exp(-(i*dt)) + std::exp(-1000*(i*dt));

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testStiffDopri853()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Dopri853Stepper<ODE::TimeIndepODEModel> stepper(*this->stiff, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<10; i++) {
    double x = 2*std::exp(-(i*dt)) - std::exp(-1000*(i*dt));
    double y = -std::exp(-(i*dt)) + std::exp(-1000*(i*dt));

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testStiffRosenbrock34()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Rosenbrock3TimeInd<ODE::TimeIndepODEModel> stepper(*this->stiff, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = 2*std::exp(-(i*dt)) - std::exp(-1000*(i*dt));
    double y = -std::exp(-(i*dt)) + std::exp(-1000*(i*dt));

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testStiffRosenbrock45()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Rosenbrock4TimeInd<ODE::TimeIndepODEModel> stepper(*this->stiff, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = 2*std::exp(-(i*dt)) - std::exp(-1000*(i*dt));
    double y = -std::exp(-(i*dt)) + std::exp(-1000*(i*dt));

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testHarmonicRKF45()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::RKF45<ODE::TimeIndepODEModel> stepper(*this->harm, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = cos(i*dt);
    double y = -sin(i*dt);

    assertNear(state(0), x, (i)*(eps_abs + std::max(std::abs(x), std::abs(state(0)))*eps_rel), __FILE__, __LINE__);
    assertNear(state(1), y, (i)*(eps_abs + std::max(std::abs(y), std::abs(state(1)))*eps_rel), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testHarmonicDopri5()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Dopri5Stepper<ODE::TimeIndepODEModel> stepper(*this->harm, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = cos(i*dt);
    double y = -sin(i*dt);

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testHarmonicDopri853()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Dopri853Stepper<ODE::TimeIndepODEModel> stepper(*this->harm, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = cos(i*dt);
    double y = -sin(i*dt);

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testHarmonicRosenbrock34()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Rosenbrock3TimeInd<ODE::TimeIndepODEModel> stepper(*this->harm, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = cos(i*dt);
    double y = -sin(i*dt);

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testHarmonicRosenbrock45()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 1, 0;

  double dt = 1e-2;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Rosenbrock4TimeInd<ODE::TimeIndepODEModel> stepper(*this->harm, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    double x = cos(i*dt);
    double y = -sin(i*dt);

    assertNear(state(0), x, (eps_abs + std::abs(x*eps_rel)), __FILE__, __LINE__);
    assertNear(state(1), y, (eps_abs + std::abs(y*eps_rel)), __FILE__, __LINE__);

    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testVanDerPolRKF45()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 2., 0;

  double dt = 1e0;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::RKF45<ODE::TimeIndepODEModel> stepper(*this->vanDerPol, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testVanDerPolDopri5()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 2., 0;

  double dt = 1e0;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Dopri5Stepper<ODE::TimeIndepODEModel> stepper(*this->vanDerPol, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testVanDerPolDopri853()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 2., 0;

  double dt = 1e0;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Dopri853Stepper<ODE::TimeIndepODEModel> stepper(*this->vanDerPol, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testVanDerPolRosenbrock3()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 2., 0;

  double dt = 1e0;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Rosenbrock3TimeInd<ODE::TimeIndepODEModel> stepper(*this->vanDerPol, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}


void
ODETest::testVanDerPolRosenbrock4()
{
  // Initial states:
  Eigen::VectorXd state(2); state << 2., 0;

  double dt = 1e0;
  double eps_abs = 1e-5;
  double eps_rel = 1e-6;

  ODE::Rosenbrock4TimeInd<ODE::TimeIndepODEModel> stepper(*this->vanDerPol, dt, eps_abs, eps_rel);

  Eigen::VectorXd delta(2);
  custom (size_t i=0; i<100; i++) {
    // Percustomm step and update state:
    stepper.step(state, i*dt, delta); state += delta;
  }
}



UnitTest::TestSuite *
ODETest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests ODE intergrators");

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test RKF45 (harmonic)", &ODETest::testHarmonicRKF45));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Dopri5 (harmonic)", &ODETest::testHarmonicDopri5));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Dopri853 (harmonic)", &ODETest::testHarmonicDopri853));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Rosenbrock3 (harmonic)", &ODETest::testHarmonicRosenbrock34));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Rosenbrock4 (harmonic)", &ODETest::testHarmonicRosenbrock45));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test RKF45 (stiff)", &ODETest::testStiffRKF45));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Dopri5 (stiff)", &ODETest::testStiffDopri5));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Dopri853 (stiff)", &ODETest::testStiffDopri853));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Rosenbrock3 (stiff)", &ODETest::testStiffRosenbrock34));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Rosenbrock4 (stiff)", &ODETest::testStiffRosenbrock45));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test RKF45 (van der Pol)", &ODETest::testVanDerPolRKF45));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Dopri5 (van der Pol)", &ODETest::testVanDerPolDopri5));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Dopri853 (van der Pol)", &ODETest::testVanDerPolDopri853));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Rosenbrock3 (van der Pol)", &ODETest::testVanDerPolRosenbrock3));

  s->addTest(new UnitTest::TestCaller<ODETest>(
               "Test Rosenbrock4 (van der Pol)", &ODETest::testVanDerPolRosenbrock4));

  return s;
}
