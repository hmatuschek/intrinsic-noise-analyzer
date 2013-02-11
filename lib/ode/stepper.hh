#ifndef __FLUC_ODE_STEPPER_HH__
#define __FLUC_ODE_STEPPER_HH__

#include <eigen3/Eigen/Eigen>


namespace iNA {
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
   * Destructor
   */
   virtual ~Stepper()
   {
      //pass...
   }

   /**
   * Defines the virtual step method needed to be implemented by any stepper algorithm.
   */
  virtual void step(const Eigen::VectorXd &state, double t, Eigen::VectorXd &delta) = 0;

  /**
   * Defines a second virtual step method that uses the stepper implemented by the algorithm.
   */
  virtual void step(Eigen::VectorXd &state, double t)
  {
      Eigen::VectorXd delta(state.size());
      step(state,t,delta);
      state+=delta;
  }

   /**
   * Resets the status.
   */
   virtual  void reset()
   {
        // ...pass by default
   }

   /**
   * Signals a parameter change.
   */
   virtual void parameterChanged()
   {
      // ...pass by default
   }

};


}
}

#endif // STEPPER_HH
