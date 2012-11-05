#ifndef LNATEST_HH
#define LNATEST_HH

#include "unittest.hh"
#include "models/LNAmodel.hh"


namespace iNA {

/**
 * This class integrates some systems custom some time and compares the result
 * of the GiNaC direct evaluation and the bytecode interpreter.
 */
class LNATest : public UnitTest::TestCase
{
public:

  virtual ~LNATest();
  void testEnzymeKineticsOpen();
  void testDimerization();
  void testDimerization2();




public:
  static UnitTest::TestSuite *suite();


private:
  void compareIntegrators(const std::string &file, double final_time);

  void integrateViaByteCode(Models::LNAmodel &model,
                            const Eigen::VectorXd &init_state, Eigen::VectorXd &final_state,
                            double final_time, double err_abs, double err_rel);
};


}


#endif
