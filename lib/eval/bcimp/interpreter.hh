#ifndef __FLUC_EVALUATE_BCIMP_INTERPRETER_HH__
#define __FLUC_EVALUATE_BCIMP_INTERPRETER_HH__

#include "code.hh"
#include "eval/bci/interpreter.hh"


namespace Fluc {
namespace Evaluate {
namespace bcimp {


/**
 * This class implements the parallel execution of big systems by ditributing the evaluation
 * of a vector of expression over several threads.
 *
 * @ingroup eval
 */
template <class InType, class OutType = InType>
class Interpreter
{
protected:
  /**
   * Holds a vector of interpreters, one for each thread.
   */
  std::vector< bci::Interpreter<InType, OutType> > interpreters;


public:
  /**
   * Constructs an "empty" interpreter.
   *
   * Use @c setCode to assign some byte-code to the interpreter.
   */
  Interpreter()
  {
    // Pass...
  }

  /**
   * Constructs an interpreter.
   */
  Interpreter(Code *code)
  {
    this->setCode(code);
  }

  /**
   * (Re-) Sets the byte-code.
   */
  void setCode(Code *code)
  {
    // Instantiate enough interpreters for each thread.
    this->interpreters.resize(code->getNumThreads());
    // Assign some byte-code to each of them.
    for (size_t i=0; i<code->getNumThreads(); i++) {
      this->interpreters[i].setCode(&code->getCode(i));
    }
  }

  /**
   * Executes all interpreters in separate threads.
   */
  inline void run(const InType &input, OutType &output)
  {
#pragma omp parallel for
    for (size_t i=0; i<this->interpreters.size(); i++)
    {
      this->interpreters[i].run(input, output);
    }
  }
};


}
}
}

#endif // INTERPRETER_HH
