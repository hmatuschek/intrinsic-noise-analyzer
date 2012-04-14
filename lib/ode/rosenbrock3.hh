#ifndef __FLUC_ODE_ROSENBROCK_HH__
#define __FLUC_ODE_ROSENBROCK_HH__


#include <eigen3/Eigen/Eigen>
#include <limits>
#include "exception.hh"
#include "stepper.hh"
#include "math.hh"



namespace Fluc {
namespace ODE {


/**
 * @cond 0
 * (excluded from docs)
 *
 * Just a simple namespace to hold all constants used by the Rosenbrock methods.
 */
class Rosenbrock3Constants {
public:
  const static double gamma;

  const static double a21, c2,c21;
  const static double a31,a32, c3,c31,c32;
  const static double a41,a42, c4,c41,c42,c43;

  const static double b31,b32,b33;
  const static double b41,b42,b43,b44;

  const static double e1,e2,e3,e4;
};
/// @endcond



/**
 * Stepper for the implicit Rosenbrock method of 3th order, for stiff, not-explicit
 * time-dependent ODE systems.
 *
 * Constants and implementation are taken from @cite shampine1982 as:
 *
 * @f[
 * \begin{aligned}
 *  y'   &= f(t,y) \\
 *  E    &= I-\frac{1}{2}h\frac{\partial f}{\partial y}(t_0, y_0) \\
 *  f_t  &= \frac{\partial f}{\partial t}(t_0, y_0) \\
 *  Ek_1 &= f(t_0, y_0) + \frac{1}{2}hf_t \\
 *  Ek_2 &= f(t_0+h, y_0+hk_1) - \frac{3}{2}hf_t - 4k_1 \\
 *  Ek_3 &= f(t_0+\frac{3}{5}h, y_0+\frac{24}{25}hk_1+\frac{3}{35}hk_2) + \frac{121}{50}h*f_t
 *          + \frac{186}{25}k1 + \frac{6}{5}k_2 \\
 *  Ek_4 &= f(t_0+\frac{3}{5}h, y_0+\frac{24}{25}hk_1+\frac{3}{35}hk_2) + \frac{29}{250}hf_t
 *          - \frac{56}{125}k_1 - \frac{27}{125}k_2 - \frac{1}{5}k_3 \\
 *  y_3(t_0 + h) &= y_0 + h\left(\frac{98}{108}k1 + \frac{11}{72}k_2 + \frac{25}{216}k_3\right) \\
 *  y_4(t_0 + h) &= y_0 + h\left(\frac{19}{18}k1 + \frac{1}{4}k_2 + \frac{25}{216}k_3
 *                               + \frac{125}{216}k_4\right) \\
 *  \Delta y &= h\left(\frac{17}{108}k_1 + \frac{7}{72}k_2 + \frac{125}{216}k_3\right)
 * \end{aligned}
 * @f]
 *
 * @todo May also have a look in "Modified Rosenbrock methods for stiff systems."
 * 1982 by H. Shintani in "Hiroschima Mathematical Journal".
 *
 * In contrast to the implementation shown in @cite press2007, this implementation uses the maximum
 * norm to estimate the truncation-error of the integration-step.
 *
 * @ingroup ode
 */
template <class Sys>
class Rosenbrock3TimeInd
    : public Stepper, protected Rosenbrock3Constants
{
protected:
  /** Holds a weak reference to the system to be integrated. */
  Sys &system;
  /** Holds the step-size (dt). */
  const double step_size;
  /** Holds the max absolute error. */
  const double err_abs;
  /** Holds the max relative error. */
  const double err_rel;
  /** Holds the jacobian of the system, will be recalculated at each step. */
  Eigen::MatrixXd jacobian;
  Eigen::VectorXd tempState;  ///< Some temporary state.
  Eigen::VectorXd tempState2; ///< Some temporary state.
  Eigen::VectorXd k1;         ///< Some temporary state.
  Eigen::VectorXd k2;         ///< Some temporary state.
  Eigen::VectorXd k3;         ///< Some temporary state.
  Eigen::VectorXd k4;         ///< Some temporary state.
  Eigen::VectorXd yerr;       ///< The error vector.


public:
  /**
   * Constructs a Rosenbrock 3rd order stepper.
   *
   * @param system Specifies the ODE system to be integrated. This class need to implement
   *        two methods: @c evaluate, which evaluates the system and @c evaluateJacobian, which
   *        evaluates the Jacobian of the system.
   * @param dt Specifies the minimum time-step.
   * @param err_abs Specifies the max. absolute error.
   * @param err_rel Specifies the max. relative error.
   */
  Rosenbrock3TimeInd(Sys &system, double dt, double err_abs, double err_rel)
    : system(system), step_size(dt), err_abs(err_abs), err_rel(err_rel),
      jacobian(system.getDimension(), system.getDimension()),
      tempState(system.getDimension()), tempState2(system.getDimension()),
      k1(system.getDimension()), k2(system.getDimension()),
      k3(system.getDimension()), k4(system.getDimension()), yerr(system.getDimension())
  {
    // Pass...
  }


  /**
   * Performs the step, implements @c Stepper interface.
   *
   * This method calculates y(t+dt), given y(t), on exit the difference (y(t+dt)-y(t)) is stored
   * in @c step.
   *
   * @param[in] state Specifies the state of the ODE system at time @c t.
   * @param[in] t Specifies the time.
   * @param[out] step On exit, holds the update from y(t) -> y(t+dt).
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &step)
  {
    control(state, t, step, step_size);
  }


protected:
  /**
   * Implements the step-size control.
   *
   * @todo Think of an general stepper function, that returns some information about the
   *       error made in a integration step. Then, implement a abstract adaptive step-size control.
   */
  inline void control(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    if (dt <= t*std::numeric_limits<double>::epsilon()) {
      RuntimeError err;
      err << __FILE__ << " at line " << (unsigned int) __LINE__ << ": "
          << "Step-size unterflow in Rosenbrock3 stepper.";
      throw err;
    }

    double err = _step(state, t, delta, dt);
    if (Math::isNotValue(err) || 1. < err)
    {
      Eigen::VectorXd tmp_delta(this->system.getDimension());
      control(state, t, delta, dt/2);
      control(state + delta, t+dt/2, tmp_delta, dt/2);
      delta += tmp_delta;
    }
  }


  /**
   * The actual singe-step algorithm.
   */
  inline double _step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta, double dt)
  {
    // Evaluate Jacobian at t and compute LU decomposition with partial pivoting from
    // (I/(h*gamma) - Jacobian):
    system.evaluateJacobian(state, t, jacobian);
    jacobian = ((Eigen::MatrixXd::Identity(system.getDimension(), system.getDimension()) /
                 (gamma*dt)) - jacobian);
    Eigen::PartialPivLU<Eigen::MatrixXd> luJacobian(jacobian);

    // compute k1:
    system.evaluate(state, t, delta);
    k1 = luJacobian.solve(delta);

    // compute k2:
    system.evaluate(state + a21*k1, t+c2*dt, delta);
    k2 = luJacobian.solve(delta + c21*k1/dt);

    // compute k3:
    system.evaluate(state + a31*k1 + a32*k2, t+c3*dt, delta);
    k3 = luJacobian.solve(delta + (c31*k1 + c32*k2)/dt);

    // compute k4:
    system.evaluate(state + a41*k1 + a42*k2, t+c4*dt, delta);
    k4 = luJacobian.solve(delta + (c41*k1 + c42*k2 + c43*k3)/dt);

    // Calculate step
    delta.noalias() = b41*k1 + b42*k2 + b43*k3 + b44*k4;

    // Estimate error 3/4-th order:
    yerr.noalias() = e1*k1 + e2*k2 + e4*k4;

    // Return error estimate:
    double err = 0.0, sk;
    for (size_t i=0; i<system.getDimension(); i++) {
      if (Math::isNotValue(yerr[i])) {
        return std::numeric_limits<double>::infinity();
      }

      sk = err_abs + err_rel*std::max(std::abs(state(i)), std::abs(state(i)+delta(i)));
      err = std::max(err, std::abs(yerr(i))/sk);
    }

    return err;
  }
};





}
}

#endif // ROSENBROCK_HH
