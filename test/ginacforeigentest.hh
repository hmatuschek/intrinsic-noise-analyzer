#ifndef GINACFOREIGENTEST_HH
#define GINACFOREIGENTEST_HH

#include "ginacsupportforeigen.hh"
#include "unittest.hh"


namespace Fluc {


/**
 * Testing the use of Ginac expression with Eigen matrices.
 */
class GinacForEigenTest :
        public UnitTest::TestCase
{
public:
  void testMatrixVectorMultiplication();
  void testMatrixMatrixMultiplication();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // GINACFOREIGENTEST_HH
