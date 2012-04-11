#include "main.hh"
#include <iostream>

#include "unittest.hh"
#include "lnatest.hh"
#include "interpretertest.hh"
#include "ginacforeigentest.hh"
#include "regression_test.hh"
#include "mathtest.hh"
#include "mersennetwistertest.hh"
#include "sbmlshparsertest.hh"
#include "optionparsertest.hh"
#include "odetest.hh"

#if WITH_EXECUTION_ENGINE_LIBJIT
#include "libjitinterpretertest.hh"
#endif


using namespace Fluc;
using namespace Fluc::UnitTest;


int main(int argc, char *argv[])
{

  // Construct test-runner
  TestRunner runner(std::cout);

  // Add test suites:
  runner.addSuite(GinacForEigenTest::suite());
  runner.addSuite(InterpreterTest::suite());
  runner.addSuite(MathTest::suite());
  runner.addSuite(ODETest::suite());
  runner.addSuite(LNATest::suite());
  runner.addSuite(MersenneTwisterTest::suite());
  runner.addSuite(RegressionTest::suite());
  runner.addSuite(SBMLSHParserTest::suite());
  runner.addSuite(OptionParserTest::suite());

#if WITH_EXECUTION_ENGINE_LIBJIT
  runner.addSuite(LibJitInterpreterTest::suite());
#endif

  // Exec tests:
  runner();

  return 0;
}
