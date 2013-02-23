#ifndef IOSTEST_HH
#define IOSTEST_HH

#include "unittest.hh"
#include <models/IOSmodel.hh>

namespace iNA {

class IOSTest : public UnitTest::TestCase
{
public:
  /** Destructor, does nothing. */
  virtual ~IOSTest();

  /** Performs a short IOS analysis on the regression-tests/enzymekinetics1.xml model. */
  void testEnzymeKineticsBCI();
  /** Performs a short IOS analysis on the regression-tests/enzymekinetics1.xml model. */
  void testEnzymeKineticsJIT();

public:
  /** Assembles the test-suite. */
  static UnitTest::TestSuite *suite();

private:
  // Performs the integration on the given model using the BCI
  void integrateViaByteCode(Models::IOSmodel &model);
  // Performs the integration on the given model using the JIT
  void integrateViaJIT(Models::IOSmodel &model);
};


}

#endif // IOSTEST_HH
