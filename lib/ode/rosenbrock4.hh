#ifndef __FLUC_ODE_ROSENBROCK4_HH__
#define __FLUC_ODE_ROSENBROCK4_HH__

#include <eigen3/Eigen/Eigen>
#include <limits>
#include "exception.hh"
#include "stepper.hh"
#include "math.hh"



namespace Fluc {
namespace ODE {


/**
 * @cond 0
 * (exclude from docs)
 *
 * Just the constants needed by the rosenbrock 4/5 method.
 */
class Rosenbrock4Constants {
public:
  static const double c2, c3, c4;

  static const double bet2p,bet3p,bet4p;

  static const double a21, c21;
  static const double a31,a32, c31,c32;
  static const double a41,a42,a43, c41,c42,c43;
  static const double a51,a52,a53,a54, c51,c52,c53,c54;

  static const double gamma;

  static const double d1,d2,d3,d4;
  static const double c61,c62,c63,c64,c65;
  static const double d21,d22,d23,d24,d25;
  static const double d31,d32,d33,d34,d35;
};
/// @endcond



/**
 * Implements the Rosenbrock method of 4th order, as described in @cite press2007.
 *
 * In contrast to the implementation shown in @cite press2007, this implementation uses the maximum
 * norm to estimate the truncation-error of the integration-step.
 *
 * @ingroup ode
 */
template<class Sys>
class Rosenbrock4TimeInd
    : public Stepper, protected Rosenbrock4Constants
{
 protected:
  /** Holds a weak reference to the ODE system. */
  Sys &system;
  /** Holds the default step-size. */
  double step_size;
  /** Holds the absolute error. */
  double err_abs;
  /** Holds the relative error. */
  double err_rel;
  /** Will hold the Jacobian of the system. */
  Eigen::MatrixXd jacobian;
  Eigen::VectorXd tempState;    ///< Some temporary state.
  Eigen::VectorXd tempState2;   ///< Some temporary state.
  Eigen::VectorXd k1;           ///< Some temporary state.
  Eigen::VectorXd k2;           ///< Some temporary state.
  Eigen::VectorXd k3;           ///< Some temporary state.
  Eigen::VectorXd k4;           ///< Some temporary state.
  Eigen::VectorXd k5;           ///< Some temporary state.
  Eigen::VectorXd yerr;         ///< Some temporary vector, holding the error.


public:
  /**
   * Constructs a Rosenbrock driver.
   *
   * @param system Specifies the ODE system to be integrated. This class need to implement
   *        two methods: @c evaluate, which evaluates the system and @c evaluateJacobian, which
   *        evaluates the Jacobian of the system.
   * @param dt Specifies the minimum time-step.
   * @param err_abs Specifies the max. absolute error.
   * @param err_rel Specifies the max. relative error.
   */
  Rosenbrock4TimeInd(Sys &system, double dt, double err_abs, double err_rel)
    : system(system), step_size(dt), err_abs(err_abs), err_rel(err_rel),
      jacobian(system.getDimension(), system.getDimension()),
      tempState(system.getDimension()), tempState2(system.getDimension()),
      k1(system.getDimension()), k2(system.getDimension()),
      k3(system.getDimension()), k4(system.getDimension()),
      yerr(system.getDimension())
  {
    // Pass...
  }


  /**
   * Performs the step.
   *
   * This method calculates y(t+dt), given y(t), on exit the difference (y(t+dt)-y(t)) is stored
   * in @c step.
   *
   * @param[in] state Specifies the state of the ODE system at time @c t.
   * @param[in] t Specifies the time.
   * @param[out] delta On exit, holds the update from y(t) -> y(t+dt).
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta)
  {
    stepSizeControl(state, t, delta, step_size);
  }


protected:
  /**
   * Implements the step-size control.
   *
   * @todo Think of an general stepper function, that returns some information about the
   *       error made in a integration step. Then, implement a abstract adaptive step-size control.
   */
  inline void stepSizeControl(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta,
                              double dt)
  {
    double err = _step(state, t, delta, dt);
    if (Math::isNotValue<>(err) || 1. < err)
    {
      Eigen::VectorXd tmp_delta(this->system.getDimension());
      stepSizeControl(state, t, delta, dt/2);
      stepSizeControl(state+delta, t+dt/2, tmp_delta, dt/2);
      delta += tmp_delta;
    }
  }


  /**
   * Actually implements the single Rosenbrock step.
   */
  inline double _step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    // Evaluate Jacobian at t:
    system.evaluateJacobian(state, t, jacobian);

    // compute LU decomposition with partial pivoting from (I/(h*gamma) - Jacobian):
    jacobian = ((Eigen::MatrixXd::Identity(system.getDimension(), system.getDimension()) /
                 (gamma*dt)) - jacobian);
    Eigen::PartialPivLU<Eigen::MatrixXd> luJacobian(jacobian);

    // Calc k1:
    system.evaluate(state, t, delta);
    k1 = luJacobian.solve(delta);

    // Calc k2:
    system.evaluate(state+a21*k1, t+c2*dt, delta);
    k2 = luJacobian.solve(delta + c21*k1/dt);

    // Calc k3:
    system.evaluate(state+a31*k1+a32*k2, t+c3*dt, delta);
    k3 = luJacobian.solve(delta + (c31*k1 + c32*k2)/dt);

    // Calc k4:
    system.evaluate(state+a41*k1+a42*k2+a43*k3, t+c4*dt, delta);
    k4 = luJacobian.solve(delta + (c41*k1 + c42*k2 + c43*k3)/dt);

    // calc k5:
    tempState.noalias() = a51*k1 + a52*k2 + a53*k3 + a54*k4;
    system.evaluate(state+tempState, t+dt, delta);
    k5 = luJacobian.solve(delta + (c51*k1 + c52*k2 + c53*k3 + c54*k4)/dt);

    // calc yerr:
    tempState += k5;
    system.evaluate(state+tempState, t+dt, delta);
    yerr = luJacobian.solve(delta + (c61*k1 + c62*k2 + c63*k3 + c64*k4 + c65*k5)/dt);
    delta = tempState + yerr;

    // Return error estimate:
    double err = 0.0, sk;
    for (size_t i=0; i<system.getDimension(); i++) {
      if (Math::isNotValue(yerr[i])) {
        return std::numeric_limits<double>::infinity();
      }

      sk = err_abs + err_rel*std::max(std::abs(state(i)), std::abs(state(i) + delta(i)));
      err = std::max(err, std::abs(yerr(i))/sk);
    }

    return err;
  }
};


}
}

#endif // ROSENBROCK4_HH
