#ifndef __FLUC_ODE_DOPRI853_HH__
#define __FLUC_ODE_DOPRI853_HH__

#include <limits>
#include <iostream>
#include <eigen3/Eigen/Eigen>
#include "exception.hh"
#include "stepper.hh"
#include "math.hh"


namespace iNA {
namespace ODE {


/**
 * @cond 0
 * (exclude from docs)
 *
 * Defines the constants for the @c Dopri853 stepper.
 */
class Dopri853Constants
{
public:
  static const double c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c14,c15,c16,
  b1,b6,b7,b8,b9,b10,b11,b12,bhh1,bhh2,bhh3,
  er1,er6,er7,er8,er9,er10,er11,er12,
  a21,a31,a32,a41,a43,a51,a53,a54,a61,a64,a65,a71,a74,a75,a76,
  a81,a84,a85,a86,a87,a91,a94,a95,a96,a97,a98,a101,a104,a105,
  a106,a107,a108,a109,a111,a114,a115,a116,a117,a118,a119,a1110,
  a121,a124,a125,a126,a127,a128,a129,a1210,a1211,a141,a147,a148,
  a149,a1410,a1411,a1412,a1413,a151,a156,a157,a158,a1511,a1512,
  a1513,a1514,a161,a166,a167,a168,a169,a1613,a1614,a1615,
  d41,d46,d47,d48,d49,d410,d411,d412,d413,d414,d415,d416,d51,d56,
  d57,d58,d59,d510,d511,d512,d513,d514,d515,d516,d61,d66,d67,d68,
  d69,d610,d611,d612,d613,d614,d615,d616,d71,d76,d77,d78,d79,
  d710,d711,d712,d713,d714,d715,d716;
};
/// @endcond



/**
 * Implements the Dormand-Prince Runge-Kutta method of 8th order, as described in
 * @cite press2007.
 *
 * In contrast to the implementation shown in @cite press2007, this implementation uses the maximum
 * norm to estimate the truncation-error of the integration-step.
 *
 * @ingroup ode
 */
template <class Sys>
class Dopri853Stepper
    : public Stepper, protected Dopri853Constants
{
protected:
  /** Holds a weak reference to the system of ODEs. */
  Sys &system;
  /** Holds the default step-size. */
  double step_size;
  /** Holds the maximum absolute error. */
  double err_abs;
  /** Holds the maximum relative error. */
  double err_rel;
  Eigen::VectorXd k1;  ///< Some temporary state.
  Eigen::VectorXd k2;  ///< Some temporary state.
  Eigen::VectorXd k3;  ///< Some temporary state.
  Eigen::VectorXd k4;  ///< Some temporary state.
  Eigen::VectorXd k5;  ///< Some temporary state.
  Eigen::VectorXd k6;  ///< Some temporary state.
  Eigen::VectorXd k7;  ///< Some temporary state.
  Eigen::VectorXd k8;  ///< Some temporary state.
  Eigen::VectorXd k9;  ///< Some temporary state.
  Eigen::VectorXd k10; ///< Some temporary state.
  Eigen::VectorXd yerr5; ///< Holds the error estimate for the 5th order step.
  Eigen::VectorXd yerr3; ///< Holds the error estimate for the 3rd order step.


public:
  /**
   * Constructs a Dormand-Prince RK stepper.
   *
   * @param system Specifies the ODE system.
   * @param dt Specifies the default step-size.
   * @param epsilon_abs Specifies the maximum absolute error.
   * @param epsilon_rel Specifies the maximum relative error.
   */
  Dopri853Stepper(Sys &system, double dt, double epsilon_abs, double epsilon_rel)
    : system(system), step_size(dt), err_abs(epsilon_abs), err_rel(epsilon_rel),
      k1(system.getDimension()), k2(system.getDimension()), k3(system.getDimension()),
      k4(system.getDimension()), k5(system.getDimension()), k6(system.getDimension()),
      k7(system.getDimension()), k8(system.getDimension()), k9(system.getDimension()),
      k10(system.getDimension()),
      yerr5(system.getDimension()), yerr3(system.getDimension())

  {
    // Pass...
  }

  /**
   * Implements the @c Stepper interface, performs a integration step from t to t+dt
   * with adaptive step-size control.
   *
   * @param[in] state Specifies the current state.
   * @param[in] t Specifies the current time (for time-dependent systems).
   * @param[out] delta Will hold the difference between the current state and the state at t+dt.
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta)
  {
    // New step, calculate first derivative k1:
    system.evaluate(state, t, k1);

    // Now perform integration:
    control(state, t, delta, step_size);
  }


protected:
  /**
   * Implements the step-size control.
   */
  void control(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    // Check step-size:
    if (dt <= t*std::numeric_limits<double>::epsilon()) {
      RuntimeError err;
      err << __FILE__ << "(" << (unsigned int) __LINE__ << "): "
          << "Stepsize underflow in Dopri853Stepper.";
      throw err;
    }

    double err = _step(state, t, delta, dt);

    if (Math::isNotValue<>(err) || 1. < err) {
      Eigen::VectorXd tmp_delta(this->system.getDimension());
      control(state, t, delta, dt/2);
      control(state+delta, t+dt/2, tmp_delta, dt/2);
      delta += tmp_delta;
    } else {
      // Finally, determine last derivative:
      system.evaluate(state+delta, t+dt, k1);
    }
  }


  /**
   * Implements the actual single-step algorithm.
   */
  inline double _step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    // Do it...
    system.evaluate(state+dt*(a21*k1), t+c2*dt, k2);
    system.evaluate(state+dt*(a31*k1+a32*k2), t+c3*dt, k3);
    system.evaluate(state+dt*(a41*k1+a43*k3), t+c4*dt, k4);
    system.evaluate(state+dt*(a51*k1+a53*k3+a54*k4), t+c5*dt, k5);
    system.evaluate(state+dt*(a61*k1+a64*k4+a65*k5), t+c6*dt, k6);
    system.evaluate(state+dt*(a71*k1+a74*k4+a75*k5+a76*k6), t+c7*dt, k7);
    system.evaluate(state+dt*(a81*k1+a84*k4+a85*k5+a86*k6+a87*k7), t+c8*dt, k8);
    system.evaluate(state+dt*(a91*k1+a94*k4+a95*k5+a96*k6+a97*k7+a98*k8), t+c9*dt, k9);
    system.evaluate(state+dt*(a101*k1+a104*k4+a105*k5+a106*k6+a107*k7+a108*k8+a109*k9), t+c10*dt, k10);
    system.evaluate(state+dt*(a111*k1+a114*k4+a115*k5+a116*k6+a117*k7+a118*k8+a119*k9+a1110*k10), t+c11*dt, k2);
    system.evaluate(state+dt*(a121*k1+a124*k4+a125*k5+a126*k6+a127*k7+a128*k8+a129*k9+a1210*k10+a1211*k2), t+dt, k3);

    // Get diff:
    delta.noalias() = dt*(b1*k1 + b6*k6 + b7*k7 + b8*k8 + b9*k9 + b10*k10 + b11*k2 + b12*k3);

    // Calculate error:
    double err3=0.0, err5=0.0, sk, deno;
    yerr5 = delta - dt*(bhh1*k1 + bhh2*k9 + bhh3*k3);
    yerr3 = dt*(er1*k1 + er6*k6 + er7*k7 + er8*k8 + er9*k9 + er10*k10 + er11*k2 + er12*k3);

    for (size_t i=0; i<system.getDimension(); i++)
    {
      if (Math::isNotValue(yerr5[i]) || Math::isNotValue(yerr3[i])) {
        return std::numeric_limits<double>::infinity();
      }

      sk = err_abs + err_rel*std::max(std::abs(state[i]), std::abs(state[i]+delta[i]));
      err5 = std::max(err5, std::abs(yerr5[i])/sk);
      err3 = std::max(err3, std::abs(yerr3[i])/sk);
    }

    deno=err3*err3+0.01*err5*err5;
    if (deno <= 0.0)
      deno=1.0;

    /// @todo Tripple-check if this is actually the max-norm error.
    return err5*err5/std::sqrt(deno);
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

#endif // DOPRI853_HH
