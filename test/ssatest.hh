#ifndef SSATEST_HH
#define SSATEST_HH

#include "unittest.hh"


namespace iNA {

class SSATest : public UnitTest::TestCase
{
public:
  virtual ~SSATest();

  void testEnzymeKinetics();

public:
  static UnitTest::TestSuite *suite();
};

}


#endif // SSATEST_HH
