#ifndef __FLUC_ODE_RUNGEKUTTA4_HH__
#define __FLUC_ODE_RUNGEKUTTA4_HH__

#include <cstdlib>
#include <vector>
#include "stepper.hh"
#include "exception.hh"


namespace iNA {
namespace ODE {


/**
 * Implements the Runge-Kutter step of 4th order \cite press86 custom ODE integration.
 *
 * @ingroup ode
 */
template <class Sys>
class RungeKutta4
    : public Stepper
{
protected:
  /**
   * Holds a weak reference to the ODE system to integrate.
   */
  Sys &system;  

  /**
   * Points to evaluation function.
   */
  void (Sys::*evaluate)(const Eigen::VectorXd &state, double t, Eigen::VectorXd &dx);

  /**
   * Holds the step-size of the integration.
   */
  double step_size;

  /**
   * A temporary state of the system.
   */
  Eigen::VectorXd temp_state;

  /**
   * A temporary step of the system.
   */
  Eigen::VectorXd temp_diff;


public:
  /**
   * Constructs a new RK4 step custom the integration of the given system with given stepsize.
   *
   * @param system Specifies the ODE system.
   * @param dt Specifies the step-size.
   * @param _func Specifies the system-function. By default the evaluate() method of the given
   *        system will be used.
   */
  RungeKutta4(Sys &system, double dt, void(Sys::*_func)(const Eigen::VectorXd &state, double t, Eigen::VectorXd &dx)=&Sys::evaluate)
      : system(system), evaluate(_func), step_size(dt),
      temp_state(system.getDimension()), temp_diff(system.getDimension())
  {
    // Pass
  }


  /**
   * Percustomms the RK4 step.
   *
   * @param state Specifies the current state of the system.
   * @param time Specifies the current time.
   * @param delta Specifies the vector (same dimension as state), that will hold the step
   *        of the system.
   */
  virtual void step(const Eigen::VectorXd &state, double time, Eigen::VectorXd &delta)
  {

    // explicit cast
    Sys* that = (Sys*) &system;

    // Evaluate system at t:
    (*that.*evaluate)(state, time, this->temp_diff);

    // Copy delta_0
    delta = this->temp_diff;
    // Calc state_1 = state_0 + dt/2 * diff_0
    this->temp_state = state + this->temp_diff*this->step_size*0.5;

    // reeval system to get delta_1
    (*that.*evaluate)(this->temp_state, time+this->step_size*0.5, this->temp_diff);
    delta += 2.*this->temp_diff;
    // state_2 = state_0 + dt/2*diff_1
    this->temp_state = state + this->temp_diff*this->step_size*0.5;

    // reeval system to get delta_2
    (*that.*evaluate)(this->temp_state, time+this->step_size*0.5, this->temp_diff);
    delta += 2.*this->temp_diff;
    this->temp_state = state + this->step_size*this->temp_diff;

    // reeval system to get delta_3:
    (*that.*evaluate)(this->temp_state, time+this->step_size, this->temp_diff);
    // Calc final step-size as delta = (delta_0 + 2*delta_1 + 2*delta_2 + delta_3)*dt/6
    delta = (delta + this->temp_diff)*(this->step_size/6.0);

    // Check custom NaN:
    if (! (delta == delta)) {
      RuntimeError err;
      err << "Integration failed: NaN occurred during interation; reduce step-size.";
      throw err;
    }
  }

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
#endif // __FLUC_ODE_RUNGEKUTTA4_HH
