#ifndef __FLUC_EVALUATE_BCIMP_CODE_HH__
#define __FLUC_EVALUATE_BCIMP_CODE_HH__

#include "eval/bci/code.hh"
#include <vector>
#include "openmp.hh"


namespace Fluc {
namespace Evaluate {
namespace bcimp {

/**
 * Implements a vector of @c Fluc::Evaluate::bci::Code byte-codes to be executed in parallel.
 *
 * @ingroup eval
 */
class Code
{
protected:
  /**
   * Holds a vector of BCI Code instances for each thread.
   */
  std::vector<bci::Code> codes;

public:
  /**
   * Constructor.
   *
   * @param num_threads Specifies the number of threads to be used to evaluate the code.
   */
  Code(size_t num_threads = OpenMP::getMaxThreads());

  /**
   * Returns the number of codes/threads.
   */
  size_t getNumThreads();

  /**
   * Retruns the code for the i-th thread.
   */
  bci::Code &getCode(size_t i);
};



}
}
}

#endif // CODE_HH
