#ifndef __FLUC_ODE_INTEGRATIONRANGE_HH__
#define __FLUC_ODE_INTEGRATIONRANGE_HH__

#include <cstdlib>



namespace iNA {
namespace ODE {


/**
 * The integration range collects the start and end-point in time for a numerical integration, it
 * also holds the number of steps for the integration.
 *
 * @ingroup ode
 */
class IntegrationRange
{
protected:
  /**
   * Holds the start time-point.
   */
  double t_start;

  /**
   * Holds the end time-point.
   */
  double t_end;

  /**
   * Holds the number of steps.
   */
  size_t steps;


public:
  /**
   * Constructs a new integration rage.
   *
   * @param t0 Specifies the start-point in time.
   * @param t Specifies the end-point in time.
   * @param n Specifies the number of steps.
   */
  IntegrationRange(double t0, double t, size_t n);

  /**
   * Copy constructor.
   */
  IntegrationRange(const IntegrationRange &other);

  /**
   * Assignment operator.
   */
  const IntegrationRange &operator =(const IntegrationRange &other);

  /**
   * Returns the start-point in time.
   */
  double getStartTime() const;

  /**
   * Retunrs the end-point in time.
   */
  double getEndTime() const;

  /**
   * Returns the time-step.
   */
  double getStepSize() const;

  /**
   * Returns the number of integration steps.
   */
  size_t getSteps() const;
};


}
}


#endif // __FLUC_ODE_INTEGRATIONRANGE_HH__
