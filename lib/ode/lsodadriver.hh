#ifndef __FLUC_ODE_LSODEADRIVER_HH__
#define __FLUC_ODE_LSODEADRIVER__

#include <eigen3/Eigen/Eigen>
#include "exception.hh"
#include "stepper.hh"
#include "math.hh"
#include "lsoda.h"


namespace Fluc {
namespace ODE {



/**
 * Implements the Dormand-Prince Runge-Kutta method of 5th order, as described in
 * @cite press2007.
 *
 * In contrast to the implementation shown in @cite press2007, this implementation uses the maximum
 * norm to estimate the truncation-error of the integration-step.
 *
 * @ingroup ode
 */
template <class Sys>
class LsodaDriver
    : public LSODEA
{
protected:
  /** Holds a weak reference to the system of ODEs. */
  Sys &system;
  /** Holds the initial step-size (dt). */
  double step_size;
  /** Holds the maximum absolute error. */
  double err_abs;
  /** Holds the maximum relative error. */
  double err_rel;
  Eigen::VectorXd k1; ///< Some temporary state.

public:
  /**
   * Constructs a new Dormand-Prince stepper of 5th order.
   *
   * @param system Specifies the ODE system to integrate.
   * @param dt Specifies the default/maximum step-size.
   * @param epsilon_abs Specifies the absolute error for the step.
   * @param epsilon_rel Specifies the relative error for the step.
   */
  LsodaDriver(Sys &system, double dt, double epsilon_abs, double epsilon_rel)
    : system(system), LSODEA(system), step_size(dt), err_abs(epsilon_abs), err_rel(epsilon_rel),
      k1(system.getDimension())
  {
    // pass...
  }

  /**
   * Performs the step t -> t+dt.
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta)
  {
    // A new step: recalculate k1:
    system.evaluate(state, t, k1);


  }

public:
  /**
   * Returns the step-size used to integrate.
   */
  inline double getStepSize()
  {
    return this->step_size;
  }

  /**
   * Retunrs the dimension of the system.
   */
  inline size_t getDimension()
  {
    return this->system.getDimension();
  }
};


}
}

#endif // DOPRI5_HH
