#include "main.hh"

#include "intprtvsdirect.hh"


#include <iostream>

using namespace Fluc;
using namespace Fluc::Benchmark;


int main(int argc, char *argv[])
{
  // Construct test-runner
  TestRunner runner(std::cout);

  // Add test suites:
  runner.addSuite(IntprtVsDirect::suite());

  // Exec tests:
  runner();

  return 0;
}
