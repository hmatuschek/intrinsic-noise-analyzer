#ifndef LNATEST_HH
#define LNATEST_HH

#include "unittest.hh"
#include "models/LNAmodel.hh"


namespace iNA {

/** This class integrates some systems for some time and compares the result
 * of the GiNaC direct evaluation and the bytecode interpreter. */
class LNATest : public UnitTest::TestCase
{
public:
  /** Destructor. */
  virtual ~LNATest();
  /** Integrates "regression-tests/core_osc.xml" model. */
  void testCoreOSC();

public:
  /** Assembles the test suite. */
  static UnitTest::TestSuite *suite();


private:
  void compareIntegrators(const std::string &file, double final_time);

  /** Performs the actual integration. */
  void integrateViaByteCode(Models::LNAmodel &model,
                            const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                            double final_time, double err_abs, double err_rel);
};

}


#endif
