#ifndef __FLUC_ODE_SEMIIMPLICITEULER_HH__
#define __FLUC_ODE_SEMIIMPLICITEULER_HH__

#include <eigen3/Eigen/Eigen>


namespace Fluc {
namespace ODE {


/**
 * A driver for the semi-implicit Euler method.
 *
 * This method is the most simple implicit method suitable to integrate stiff systems.
 *
 * The method calculates the update as:
 *
 * \f[ \Delta y_{n+1} = y_{n+1} - y_n = h\left[I - h\frac{\partial f}{\partial y}\right]^{-1}\cdot f(y_n) \f]
 *
 * @ingroup ode
 */
template<class Sys>
class SemiImplicitEuler
{
protected:
  /**
   * Holds the system to integrate.
   */
  Sys &system;

  /**
   * Holds the step-size.
   */
  const double step_size;

  /**
   * Holds a matrix for the jacobian.
   */
  Eigen::MatrixXd jacobian;


public:
  /**
   * Constructs a new driver for the semi-implicit Euler method.
   *
   * @param system Specifies the system to integrate.
   * @param dt Specifies the step-size.
   */
  SemiImplicitEuler(Sys &system, double dt)
    : system(system), step_size(dt), jacobian(system.getDimension(), system.getDimension())
  {
    // Pass...
  }


  /**
   * Calculates the step.
   */
  inline void step(const Eigen::VectorXd &state, double time, Eigen::VectorXd &delta)
  {
    // Evaluate system and jacobian
    this->system.evaluate(state, time, delta);
    this->system.evaluateJacobian(state, time, this->jacobian);

    // Calc inverse using LU decomposition with partial pivoting
    jacobian = (Eigen::MatrixXd::Identity(system.getDimension(), system.getDimension()) - step_size*jacobian);
    Eigen::PartialPivLU<Eigen::MatrixXd> luJacobian(jacobian);

    // Calculates update step:
    delta.noalias() = (this->step_size*luJacobian.solve(delta));
  }


  /**
   * Returns the step-size used.
   */
  inline double getStepSize()
  {
    return this->step_size;
  }


  /**
   * Returns the dimension of the system.
   */
  inline size_t getDimension()
  {
    return this->system.getDimension();
  }
};


}
}

#endif // SEMIIMPLICITEULER_HH
