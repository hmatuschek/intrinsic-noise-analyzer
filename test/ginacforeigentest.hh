#ifndef GINACcustomEIGENTEST_HH
#define GINACcustomEIGENTEST_HH

#include "ginacsupportcustomeigen.hh"
#include "unittest.hh"


namespace iNA {


/**
 * Testing the use of Ginac expression with Eigen matrices.
 */
class GinaccustomEigenTest :
        public UnitTest::TestCase
{
public:
  virtual ~GinaccustomEigenTest()
  {

  }

public:
  void testMatrixVectorMultiplication();
  void testMatrixMatrixMultiplication();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // GINACcustomEIGENTEST_HH
