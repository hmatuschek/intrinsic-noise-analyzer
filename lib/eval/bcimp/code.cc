#include "code.hh"

using namespace Fluc::Evaluate;
using namespace Fluc::Evaluate::bcimp;


Code::Code(size_t num_threads)
  : codes(num_threads)
{
  // Pass...
}


size_t
Code::getNumThreads()
{
  return this->codes.size();
}


bci::Code &
Code::getCode(size_t i)
{
  return this->codes[i];
}
