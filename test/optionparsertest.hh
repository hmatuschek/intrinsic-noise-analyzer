#ifndef OPTIONPARSERTEST_HH
#define OPTIONPARSERTEST_HH


#include "unittest.hh"


namespace iNA {

class OptionParserTest : public UnitTest::TestCase
{
public:
  void testOptionParser();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // OPTIONPARSERTEST_HH
