#ifndef __FLUC_CPUTIME_HH__
#define __FLUC_CPUTIME_HH__

#include <time.h>
#include <list>


namespace Fluc {
namespace Utils {

/**
 * A utility class to measure the CPU time used by some algorithms.
 *
 * @todo Move into Utils namespace.
 */
class CpuTime
{
protected:
  /**
   * The stack of start times.
   */
  std::list< clock_t > clocks;

public:
  /**
   * Constructs a new CPU time clock.
   */
  CpuTime();

  /**
   * Start the clock.
   */
  void start();

  /**
   * Stops the clock and returns the time in seconds.
   */
  double stop();

  /**
   * Retruns the current time of the current clock.
   */
  double getTime();
};

}
}

#endif // CPUTIME_HH
