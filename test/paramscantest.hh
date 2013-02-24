#ifndef PARAMSCANTEST_HH
#define PARAMSCANTEST_HH

#include "unittest.hh"

namespace iNA {

class ParamScanTest : public UnitTest::TestCase
{
public:
  void testEnzymeKinetics();

public:
  static UnitTest::TestSuite *suite();
};

}

#endif // PARAMSCANTEST_HH
