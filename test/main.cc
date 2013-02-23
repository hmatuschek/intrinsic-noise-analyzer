#include "main.hh"
#include <iostream>

#include "unittest.hh"
#include "lnatest.hh"
#include "iostest.hh"
#include "interpretertest.hh"
#include "ginacforeigentest.hh"
#include "regression_test.hh"
#include "mathtest.hh"
#include "mersennetwistertest.hh"
#include "sbmlshparsertest.hh"
#include "optionparsertest.hh"
#include "odetest.hh"
#include "modelcopytest.hh"
#include "constantfoldertest.hh"
#include "conservationanalysistest.hh"
#include "unitparsertest.hh"
#include "benchmark.hh"
#include "benchmark_pscan.hh"
#include "expressionparsertest.hh"

#include "utils/option_parser.hh"
#include "utils/logger.hh"


using namespace iNA;
using namespace iNA::UnitTest;


int main(int argc, char *argv[])
{
  // Assemble option-grammar:
  Utils::Opt::Parser parser(
        ( Utils::Opt::Parser::zeroOrMore( Utils::Opt::Parser::Option("skip") ) |
          Utils::Opt::Parser::Flag("help")) );

  // Parse flags.
  if (! parser.parse((const char **)argv, argc)) {
    std::cerr << "Invalid arguments." << std::endl;
    std::cerr << parser.format_help(argv[0]) << std::endl;
    return 1;
  }

  // If --help is present:
  if (parser.has_flag("help")) {
    std::cerr << parser.format_help(argv[0]) << std::endl;
    return 0;
  }

  // Holds the tests to be skipped:
  std::set<std::string> skipped_tests;
  // Add tests skipped by default:
  skipped_tests.insert("ODE");
  skipped_tests.insert("RNG");
  skipped_tests.insert("Benchmark");
  skipped_tests.insert("BenchmarkPScan");
  // Extract skipped tests:
  if (parser.has_option("skip")) {
    const std::list<std::string> &names = parser.get_option("skip");
    skipped_tests.insert(names.begin(), names.end());
  }

  // Assemble logger:
  /*Utils::Logger::get().addHandler(
        new Utils::TextMessageHandler(std::cerr, Utils::Message::INFO));*/

  // Construct test-runner
  TestRunner runner(std::cout);

  // Add test suites
  if (0 == skipped_tests.count("GinacForEigen"))
    runner.addSuite(GinacForEigenTest::suite());
  if (0 == skipped_tests.count("Interpreter"))
    runner.addSuite(InterpreterTest::suite());
  if (0 == skipped_tests.count("Math"))
    runner.addSuite(MathTest::suite());
  if (0 == skipped_tests.count("ModelCopy"))
    runner.addSuite(ModelCopyTest::suite());
  if (0 == skipped_tests.count("Regression"))
    runner.addSuite(RegressionTest::suite());
  if (0 == skipped_tests.count("ExpressionParser"))
    runner.addSuite(ExpressionParserTest::suite());
  if (0 == skipped_tests.count("SBMLSH"))
    runner.addSuite(SBMLSHParserTest::suite());
  if (0 == skipped_tests.count("OptionParser"))
    runner.addSuite(OptionParserTest::suite());
  if (0 == skipped_tests.count("ConstantFolder"))
    runner.addSuite(ConstantFolderTest::suite());
  if (0 == skipped_tests.count("ConservationAnalysis"))
    runner.addSuite(ConservationAnalysisTest::suite());
  if (0 == skipped_tests.count("UnitParser"))
    runner.addSuite(UnitParserTest::suite());
  if (0 == skipped_tests.count("LNATest"))
    runner.addSuite(LNATest::suite());
  if (0 == skipped_tests.count("IOSTest"))
    runner.addSuite(IOSTest::suite());
  if (0 == skipped_tests.count("ODE"))
    runner.addSuite(ODETest::suite());
  if (0 == skipped_tests.count("RNG"))
    runner.addSuite(MersenneTwisterTest::suite());
  if (0 == skipped_tests.count("Benchmark"))
    runner.addSuite(Benchmark::suite());
  if (0 == skipped_tests.count("BenchmarkPScan"))
    runner.addSuite(BenchmarkPscan::suite());

  // Exec tests:
  runner();

  return 0;
}
