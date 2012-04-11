#ifndef __FLUC_ODE_STEPPER_HH__
#define __FLUC_ODE_STEPPER_HH__

#include <eigen3/Eigen/Eigen>


namespace Fluc {
namespace ODE {

/**
 * Pure virtual class to define the general stepper interface.
 *
 * @ingroup ode
 */
class Stepper
{
public:
  /**
   * Defines the virtual step method needed to be implemented by any stepper algorithm.
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta) = 0;
};


}
}

#endif // STEPPER_HH
