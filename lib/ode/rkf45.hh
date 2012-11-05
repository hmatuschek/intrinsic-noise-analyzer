#ifndef __FLUC_ODE_RKF45_HH__
#define __FLUC_ODE_RKF45_HH__


#include <eigen3/Eigen/Eigen>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "stepper.hh"
#include "exception.hh"
#include "math.hh"


namespace iNA {
namespace ODE {


/**
 * @cond 0
 * (excluded from docs)
 *
 * Provides some constants for the @c RKF45 stepper.
 */
class RKF45Constants {
public:
  static const double a21,a31,a32,a41,a42,a43,a51,a52,a53,a54,a61,a62,a63,a64,a65;
  static const double c2,c3,c4,c5,c6;
  static const double b31,b32,b33,b34,b35,b36;
  static const double b41,b42,b43,b44,b45,b46;
};
/// @endcond



/**
 * Implements the Runge-Kutter-Fehlberg step of 4/5th order \cite fehlberg69 \cite press86 for ODE
 * integration.
 *
 * This class implements the embedded Runge-Kutta-Fehlberg method of 4/5 order. This method embedds
 * the 4-th order integration step into the 5-th order step and allows to perform an adapdive
 * step-size control using these two order without the need of reavaluation.
 *
 * \f{align}{
 *  k_1 &= h f(t_k, y_k) \\
 *  k_2 &= h f(t_k + \frac{1}{4}h, y_k + \frac{1}{4}k_1) \\
 *  k_3 &= h f(t_k + \frac{3}{8}h, y_k + \frac{3}{32}k_1 + \frac{9}{32}k_2) \\
 *  k_4 &= h f(t_k + \frac{12}{13}h, y_k + \frac{1932}{2197}k_1 - \frac{7200}{2197}k_2 +
           \frac{7296}{2197}k_3) \\
 *  k_5 &= h f(t_k + h, y_k + \frac{439}{216}k_1 - 8k_2 + \frac{3680}{513}k_3 -
           \frac{845}{4104}k_4) \\
 *  k_6 &= h f(t_k + \frac{1}{2}h, y_k - \frac{8}{27}k_1 + 2k_2 - \frac{3544}{2565}k_3 +
           \frac{1859}{4104}k_4 - \frac{11}{40}k_5) \\
 * \f}
 *
 * Which yields the following update-steps for the 4th and 5th order:
 *
 * \f{align}{
 *  \Delta_4 &= \frac{25}{216}k_1 + \frac{1408}{2565}k_3 + \frac{2197}{4101}k_4 - \frac{1}{5}k_5 \\
 *  \Delta_5 &= \frac{16}{135}k_1 + \frac{6656}{12825}k_3 + \frac{28561}{56430}k_4 -
                \frac{9}{50}k_5 + \frac{2}{55}k_6
 * \f}
 *
 * The difference between the 5th and 4th order (\f$\epsilon = \left|\Delta_5 - \Delta_4\right|\f$)
 * can be used to determine if the stepsize \f$h\f$ needs to be adjusted. The step-size does not
 * need to be reduced if the following inequation holds true:
 *
 * \f[
 *   \left|\Delta_5 - \Delta_4\right| < \epsilon_{abs} + \epsilon_{rel}\left|y\right|
 * \f]
 *
 * where \f$\left|y\right|\f$ is the element wise maximum of \f$\left|y_k\right|\f$ and
 * \f$\left|y_{k+1}\right|\f$.
 *
 * @ingroup ode
 */
template <class Sys>
class RKF45
    : public Stepper, protected RKF45Constants
{
protected:
  /** Holds a weak reference to the ODE system to integrate. */
  Sys &system;
  /** Holds the step-size of the integration. */
  double step_size;
  /** Maximum absolute error. */
  double epsilon_abs;
  /** Maximum relative error. */
  double epsilon_rel;
  Eigen::VectorXd k1; ///< Some temporary state.
  Eigen::VectorXd k2; ///< Some temporary state.
  Eigen::VectorXd k3; ///< Some temporary state.
  Eigen::VectorXd k4; ///< Some temporary state.
  Eigen::VectorXd k5; ///< Some temporary state.
  Eigen::VectorXd k6; ///< Some temporary state.

public:
  /**
   * Constructs a new RKF45 step for the integration of the given system with given stepsize.
   *
   * @param system Specifies the ODE system.
   * @param dt Specifies the step-size.
   * @param epsilon_abs Specifies the maximum absolute error between the 4th and 5th order RK.
   * @param epsilon_rel Specifies the maximum relative error between the 4th and 5th order RK.
   */
  RKF45(Sys &system, double dt, double epsilon_abs, double epsilon_rel)
    : system(system), step_size(dt), epsilon_abs(epsilon_abs), epsilon_rel(epsilon_rel),
      k1(system.getDimension()), k2(system.getDimension()), k3(system.getDimension()),
      k4(system.getDimension()), k5(system.getDimension()), k6(system.getDimension())
  {
    // Pass
  }


  /**
   * Performs the RKF45 step.
   *
   * @param state Specifies the current state of the system.
   * @param time Specidies the current time.
   * @param delta Specifies the vector (same dimension as state), that will hold the step
   *        of the system.
   */
  virtual void step(const Eigen::VectorXd &state, double time, Eigen::VectorXd &delta)
  {
    // forward with initial step-size:
    this->control(state, time, this->step_size, delta);

    // Check for NaN:
    if (! (delta == delta)) {
      RuntimeError err;
      err << "Integration failed: NaN occurred during interation; reduce step-size.";
      throw err;
    }
  }

protected:
  /**
   * The actual stepper.
   */
  void control(const Eigen::VectorXd &state, double time, double dt, Eigen::VectorXd &delta)
  {
    if (dt < time*std::numeric_limits<double>::epsilon())
    {
      throw RuntimeError("Adaptive intergration failed.");
    }

    // Make single step and check if error is small:
    double err = this->single_step(state, time, dt, delta);
    if (Math::isNotValue<>(err) || 1. < err)
    {
      Eigen::VectorXd tmp_delta(this->system.getDimension());
      this->control(state, time, dt/2., tmp_delta);
      this->control(state+tmp_delta, time+dt/2., dt/2., delta);
      delta += tmp_delta;
    }
  }


  /**
   * Implements a single RKF45 step.
   */
  inline double single_step(const Eigen::VectorXd &state, double time, double dt, Eigen::VectorXd &delta)
  {
    this->system.evaluate(state, time, k1);
    this->system.evaluate(state + dt*(a21*k1), time+dt*c2, k2);
    this->system.evaluate(state + dt*(a31*k1 + a32*k2), time+dt*c3, k3);
    this->system.evaluate(state + dt*(a41*k1 + a42*k2 + a43*k3), time+c4*dt, k4);
    this->system.evaluate(state + dt*(a51*k1 + a52*k2 + a53*k3 + a54*k4), time+dt*c5, k5);
    this->system.evaluate(state + dt*(a61*k1 + a62*k2 + a63*k3 + a64*k4 + a65*k5), time+dt*c6, k6);

    // Calc delta t -> t+dt
    delta.noalias() = dt*(b41*k1 + b43*k3 + b44*k4 + b45*k5 + b46*k6);

    // Calc |y_err|
    k2.noalias() = (dt*(b31*k1 + b33*k3 + b34*k4 + b35*k5) - delta).cwiseAbs();

    double err=0.0, sk;
    for (size_t i=0; i<system.getDimension(); i++) {
      if (Math::isNotValue(k2[i])) {
        return std::numeric_limits<double>::infinity();
      }

      sk = epsilon_abs + epsilon_rel*std::max(std::abs(state[i]), std::abs(state[i]+delta[i]));
      err = std::max(err, std::abs(k2[i])/sk);
    }
    return err;
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
#endif // RUNGEKUTTA4_HH
