#include "optionparsertest.hh"
#include "utils/option_parser.hh"


using namespace iNA;
using namespace iNA::Utils::Opt;


void
OptionParserTest::testOptionParser()
{
  {
    Parser parser;
    parser.setGrammar(
          parser.opt( (parser.Flag("help") | parser.Flag("version")) ) );

    const char *argv_1[2] = {"program", "--version"};
    const char *argv_2[2] = {"program", "--help"};
    const char *argv_3[1] = {"program"};
    const char *argv_4[3] = {"program", "--help", "--version"};

    UT_ASSERT(parser.parse(argv_1, 2));
    UT_ASSERT(parser.parse(argv_2, 2));
    UT_ASSERT(parser.parse(argv_3, 1));
    UT_ASSERT(! parser.parse(argv_4, 3));
  }

  {
    Parser parser;
    parser.setGrammar(
          ( parser.opt( (parser.Flag("help") | parser.Flag("version")) ),
            parser.Value() ) );

    const char *argv_1[2] = {"program", "value"};
    const char *argv_2[3] = {"program", "--help", "value"};
    const char *argv_3[3] = {"program", "value", "value"};

    UT_ASSERT(parser.parse(argv_1, 2));
    UT_ASSERT(parser.parse(argv_2, 3));
    UT_ASSERT(! parser.parse(argv_3, 3));
  }
}


UnitTest::TestSuite *
OptionParserTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests Utils::Opt::Parser.");
  s->addTest(new UnitTest::TestCaller<OptionParserTest>(
               "simple test", &OptionParserTest::testOptionParser));

  return s;
}
