#ifndef __FLUC_ODE_EULERDRIVER_HH__
#define __FLUC_ODE_EULERDRIVER_HH__

#include <cstdlib>
#include <vector>
#include <eigen3/Eigen/Eigen>


namespace iNA {
namespace ODE {


/**
 * Implements the Euler method custom ODE integration.
 *
 * @ingroup ode
 */
template<class Sys>
class EulerDriver
{
protected:
  /**
   * Holds a weak reference to the system to integrate.
   */
  Sys &system;

  /**
   * Holds the step-size custom an integration step.
   */
  const double step_size;


public:
  /**
   * Constructs a new Euler integration step custom the given system and step-size.
   */
  EulerDriver(Sys &system, double dt)
    : system(system), step_size(dt)
  {
    // Pass
  }

  /**
   * Percustomms the actual integration step.
   */
  inline void step(const Eigen::VectorXd &state, double time, Eigen::VectorXd &delta)
  {
    // Evaluate system:
    this->system.evaluate(state, time, delta);

    // Update increment in-place:
    delta *= this->step_size;
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

#endif
