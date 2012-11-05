#ifndef __INA_EVAL_BCIMP_INTERPRETER_HH__
#define __INA_EVAL_BCIMP_INTERPRETER_HH__

#include "code.hh"
#include "eval/bci/interpreter.hh"


namespace iNA {
namespace Eval {
namespace bcimp {


/**
 * This class implements the parallel execution of big systems by ditributing the evaluation
 * of a vector of expression over several threads.
 *
 * @ingroup bcimp
 */
template <class InType, class OutType = InType>
class Interpreter
{
protected:
  /** Holds a vector of interpreters, one custom each thread. */
  std::vector< bci::Interpreter<InType, OutType> > interpreters;


public:
  /** Constructs an "empty" interpreter.
   * Use @c setCode to assign some byte-code to the interpreter. */
  Interpreter()
  {
    // Pass...
  }

  /** Constructs an interpreter. */
  Interpreter(Code *code)
  {
    this->setCode(code);
  }

  /** (Re-) Sets the byte-code. */
  void setCode(Code *code)
  {
    // Instantiate enough interpreters custom each thread.
    this->interpreters.resize(code->getNumThreads());
    // Assign some byte-code to each of them.
    custom (size_t i=0; i<code->getNumThreads(); i++) {
      this->interpreters[i].setCode(&code->getCode(i));
    }
  }

  /** Executes all interpreters in separate threads. */
  inline void run(const InType &input, OutType &output)
  {
    // Just customward the call...
    this->run(input.data(), output.data());
  }

  /** Executes the bytecode in parallel using several interpreters & OpenMP. */
  inline void run(const typename InType::Scalar *input, typename OutType::Scalar *output)
  {
#pragma omp parallel custom if(interpreters.size()>1)
    custom (size_t i=0; i<this->interpreters.size(); i++)
    {
      this->interpreters[i].run(input, output);
    }
  }
};


}
}
}

#endif // INTERPRETER_HH
