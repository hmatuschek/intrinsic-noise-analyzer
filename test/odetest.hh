#ifndef __FLUC_ROSENBROCKTEST_HH__
#define __FLUC_ROSENBROCKTEST_HH__

#include "unittest.hh"
#include "ode/odemodel.hh"


namespace Fluc {

/**
 * Performs some simple tests for the integrator drivers.
 */
class ODETest : public UnitTest::TestCase
{
protected:
  /**
   * Holds a simple harmonic oscillator.
   */
  ODE::TimeIndepODEModel *harm;

  /**
   * Will hold a simple stiff system.
   */
  ODE::TimeIndepODEModel *stiff;

  /**
   * Will hold the van der Pol system.
   */
  ODE::TimeIndepODEModel *vanDerPol;


public:
  ODETest();

  void testHarmonicRKF45();
  void testHarmonicDopri5();
  void testHarmonicDopri853();
  void testHarmonicRosenbrock34();
  void testHarmonicRosenbrock45();

  void testStiffRKF45();
  void testStiffDopri5();
  void testStiffDopri853();
  void testStiffRosenbrock34();
  void testStiffRosenbrock45();

  void testVanDerPolRKF45();
  void testVanDerPolDopri5();
  void testVanDerPolDopri853();
  void testVanDerPolRosenbrock3();
  void testVanDerPolRosenbrock4();

public:
  /**
   * Constructs the test case.
   */
  static UnitTest::TestSuite *suite();
};


}

#endif // ROSENBROCKTEST_HH
