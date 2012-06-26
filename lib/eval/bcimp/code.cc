#include "code.hh"
#include "utils/logger.hh"
using namespace Fluc::Eval;
using namespace Fluc::Eval::bcimp;


Code::Code(size_t num_threads)
  : codes(num_threads)
{
    Utils::Message message = LOG_MESSAGE(Utils::Message::DEBUG);
    message << "Bytecode interpreter initialized with " << num_threads
            << " threads.";
    Utils::Logger::get().log(message);
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
