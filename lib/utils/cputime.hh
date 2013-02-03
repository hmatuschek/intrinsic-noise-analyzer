#ifndef __FLUC_CPUTIME_HH__
#define __FLUC_CPUTIME_HH__

#include <time.h>
#include <sys/time.h>
#include <list>


namespace iNA {
namespace Utils {

/**
 * A utility class to measure the CPU time used by some algorithms.
 *
 * @ingroup utils
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
   * Returns the current time of the current clock.
   */
  double getTime();
};


/**
 * A utility class to measure the real time used by some algorithms.
 *
 * @ingroup utils
 */
class RealTime
{
protected:
  /**
   * The stack of start times.
   */
  std::list< struct timeval > clocks;

public:
  /**
   * Constructs a new CPU time clock.
   */
  RealTime();

  /**
   * Start the clock.
   */
  void start();

  /**
   * Stops the clock and returns the time in seconds.
   */
  double stop();

  /**
   * Returns the current time of the current clock.
   */
  double getTime();
};


}
}

#endif // CPUTIME_HH
