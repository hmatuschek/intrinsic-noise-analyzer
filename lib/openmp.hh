#ifndef __FLUC_OPENMP_WRAPPER_HH__
#define __FLUC_OPENMP_WRAPPER_HH__

#include <cstdlib>
#include "config.hh"
#ifdef INA_ENABLE_OPENMP
#include <omp.h>
#endif

/** Simple wrapper class to encapsulate OpenMP.
 * @ingroup utils */
class OpenMP
{
public:
  /** Returns the number of available threads.
   * Returns 1, if OpenMP is disabled. */
  static inline size_t getMaxThreads() {
#ifdef _OPENMP
    return omp_get_max_threads();
#endif
    return 1;
  }

  /** Returns the thread-identifier or 0 if OpenMP is disabled. */
  static size_t getThreadNum() {
#ifdef _OPENMP
    return omp_get_thread_num();
#endif
    return 0;
  }
};

#endif // OPENMP_HH
