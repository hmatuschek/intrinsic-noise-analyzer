#ifndef __FLUC_ODE_LSODEADRIVER_HH__
#define __FLUC_ODE_LSODEADRIVER__

#include <eigen3/Eigen/Eigen>
#include "exception.hh"
#include "stepper.hh"
#include "math.hh"
#include "lsoda.hh"


namespace Fluc {
namespace ODE {



/**
 * Drives the LSODA integrator *
 * In contrast to the implementation shown in @cite press2007, this implementation uses the maximum
 * norm to estimate the truncation-error of the integration-step.
 *
 * @ingroup ode
 */
template <class Sys>
class LsodaDriver
    : public Stepper, protected LSODA
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

  //Eigen::VectorXd dx;

  double *ywork;
  double *atolwork;
  double *rtolwork;

public:
  /**
   * Constructs a new LSODA stepper.
   *
   * @param system Specifies the ODE system to integrate.
   * @param dt Specifies the default/maximum step-size.
   * @param epsilon_abs Specifies the absolute error for the step.
   * @param epsilon_rel Specifies the relative error for the step.
   */
  LsodaDriver(Sys &system, double dt, double epsilon_abs, double epsilon_rel)
      : system(system), step_size(dt), err_abs(epsilon_abs), err_rel(epsilon_rel)
  {
    istate=1;

    ywork = new double[3 * (getDimension() + 1)];
    atolwork = ywork + getDimension() + 1;
    rtolwork = atolwork + getDimension() + 1;

    /* @todo use to define staged errors using the system size */
    for (size_t i = 1; i <= getDimension(); ++i)
    {
        rtolwork[i] = err_rel;
        atolwork[i] = err_abs;
    }
 }

  /**
   * Performs the step t -> t+dt.
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta)
  {
      for (size_t i = 1; i <= getDimension(); ++i) ywork[i] = state[i-1];
      lsoda(getDimension(), ywork, &t, t+step_size, 2, rtolwork, atolwork, 1, &istate, 0, 2);
      for (size_t i = 1; i <= getDimension(); ++i) delta[i-1] = ywork[i]-state[i-1];
  }

  virtual void step(Eigen::VectorXd &state, double t)
  {
      //for (size_t i = 1; i <= getDimension(); ++i) ywork[i] = state[i-1];
      lsoda(getDimension(), state.data()-1, &t, t+step_size, 2, rtolwork, atolwork, 1, &istate, 0, 2);
      //for (size_t i = 1; i <= getDimension(); ++i) delta[i-1] = ywork[i]-state[i-1];
  }

  virtual void evalODE(double t, double y[], double yd[], int nsize)
  {
      system.evaluate(y, t, yd);
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

#endif
