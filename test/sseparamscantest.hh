#ifndef SSEPARAMSCANTEST_HH
#define SSEPARAMSCANTEST_HH

#include "unittest.hh"

namespace iNA {

class SSEParamScanTest : public UnitTest::TestCase
{
public:
  virtual ~SSEParamScanTest();

  void testEnzymeKinetics();

public:
  static UnitTest::TestSuite *suite();
};

}

#endif // SSEPARAMSCANTEST_HH
