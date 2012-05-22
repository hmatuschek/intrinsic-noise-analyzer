#ifndef LNATEST_HH
#define LNATEST_HH

#include "unittest.hh"
#include "models/linearnoiseapproximation.hh"


namespace Fluc {

/**
 * This class integrates some systems for some time and compares the result
 * of the GiNaC direct evaluation and the bytecode interpreter.
 */
class LNATest : public UnitTest::TestCase
{
public:
  void testEnzymeKineticsOpen();
  void testDimerization();
  void testDimerization2();


public:
  static UnitTest::TestSuite *suite();


private:
  void compareIntegrators(const std::string &file, double final_time);

  void integrateViaByteCode(Models::LinearNoiseApproximation &model,
                            const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                            double final_time, double err_abs, double err_rel);
};


}


#endif
