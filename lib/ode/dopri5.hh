#ifndef __FLUC_ODE_DOPRI5_HH__
#define __FLUC_ODE_DOPRI5_HH__

#include <limits>
#include <eigen3/Eigen/Eigen>
#include "exception.hh"
#include "stepper.hh"
#include "math.hh"


namespace Fluc {
namespace ODE {


/**
 * @cond 0
 * (excluded from docs)
 *
 * Provides some constants used in @c Dopri5Stepper.
 */
class Dopri5Constants
{
public:
  static const double  c2, c3, c4, c5;

  static const double a21;
  static const double a31,a32;
  static const double a41,a42,a43;
  static const double a51,a52,a53,a54;
  static const double a61,a62,a63,a64,a65;
  static const double a71,a72,a73,a74,a75,a76;
  static const double  e1, e2, e3, e4, e5, e6, e7;
};
/// @endcond



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
class Dopri5Stepper
    : public Stepper, protected Dopri5Constants
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
  Eigen::VectorXd k2; ///< Some temporary state.
  Eigen::VectorXd k3; ///< Some temporary state.
  Eigen::VectorXd k4; ///< Some temporary state.
  Eigen::VectorXd k5; ///< Some temporary state.
  Eigen::VectorXd k6; ///< Some temporary state.
  Eigen::VectorXd last_diff; ///< Some temporary state.
  Eigen::VectorXd yerr; ///< Some temporary state.
  Eigen::VectorXd temp; ///< Some temporary state.


public:
  /**
   * Constructs a new Dormand-Prince stepper of 5th order.
   *
   * @param system Specifies the ODE system to integrate.
   * @param dt Specifies the default/maximum step-size.
   * @param epsilon_abs Specifies the absolute error for the step.
   * @param epsilon_rel Specifies the relative error for the step.
   */
  Dopri5Stepper(Sys &system, double dt, double epsilon_abs, double epsilon_rel)
    : system(system), step_size(dt), err_abs(epsilon_abs), err_rel(epsilon_rel),
      k1(system.getDimension()), k2(system.getDimension()), k3(system.getDimension()),
      k4(system.getDimension()), k5(system.getDimension()), k6(system.getDimension()),
      last_diff(system.getDimension()), yerr(system.getDimension()), temp(system.getDimension())
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
    control(state, t, delta, step_size);
  }


protected:
  /**
   * Implements the step-size control.
   */
  void control(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    // Check step-size:
    if (std::abs(dt) <= std::abs(t)*std::numeric_limits<double>::epsilon()) {
      RuntimeError err;
      err << __FILE__ << "(" << __LINE__ << "): " << "Stepsize underflow in Dopri5Stepper.";
      throw err;
    }

    // Perform step:
    double err = _step(state, t, delta, dt);
    if (Math::isNotValue<>(err) || 1. < err) {
      Eigen::VectorXd tmp_delta(this->system.getDimension());
      control(state, t, delta, dt/2);
      control(state+delta, t, tmp_delta, dt/2);
      delta += tmp_delta;
    }

    // Copy last-diff to k1:
    k1.noalias() = last_diff;
  }


  /**
   * Implements Dopri5 single step.
   */
  inline double _step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    // Reuse k1 from last call...

    system.evaluate(state + dt*(a21*k1), t+c2*dt, k2);
    system.evaluate(state + dt*(a31*k1 + a32*k2), t+c3*dt, k3);
    system.evaluate(state + dt*(a41*k1 + a42*k2 + a43*k3), t+c4*dt, k4);
    system.evaluate(state + dt*(a51*k1 + a52*k2 + a53*k3 + a54*k4), t+c5*dt, k5);
    system.evaluate(state + dt*(a61*k1 + a62*k2 + a63*k3 + a64*k4 + a65*k5), t+dt, k6);

    // Calc delta:
    delta.noalias() = dt*(a71*k1 + a72*k2 + a73*k3 + a74*k4 + a75*k5 + a76*k6);

    // Evaluate at correct t+dt, this will become k1 if the step was successfull:
    system.evaluate(state + delta, t+dt, last_diff);

    // Calc yerr:
    yerr.noalias() = dt*(e1*k1 + e2*k2 + e3*k3 + e4*k4 + e5*k5 + e6*k6 + e7*last_diff);

    // Return error estimate:
    double err = 0.0, sk;
    for (size_t i=0; i<system.getDimension(); i++) {
      if (Math::isNotValue(yerr[i])) {
        return std::numeric_limits<double>::infinity();
      }

      sk = err_abs + err_rel*std::max(std::abs(state(i)), std::abs(state(i)+delta(i)));
      err = std::max(err, std::abs(yerr[i])/sk);
      //err += (yerr(i)*yerr(i))/(sk*sk);
    }
    return err;
    //return sqrt(err/system.getDimension());
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
