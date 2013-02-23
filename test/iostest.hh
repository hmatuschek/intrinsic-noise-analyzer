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

  /** Performs a short IOS analysis on the regression-tests/core_osc.xml model. */
  void testCoreModel();

public:
  /** Assembles the test-suite. */
  static UnitTest::TestSuite *suite();

private:
  // Performs the integration on the given model:
  void integrateViaByteCode(Models::IOSmodel &model,
                            const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                            double final_time, double err_abs, double err_rel);
};


}

#endif // IOSTEST_HH
