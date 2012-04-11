#ifndef LINALGTEST_HH
#define LINALGTEST_HH

#include "unittest.hh"


namespace Fluc {

class LinalgTest: public UnitTest::TestCase
{
public:
  /**
   * Tests BLAS wrapper.
   */
  void testGemv();
  void testGemm();

  /**
   * Tests simplified interface.
   */
  void testDotMV();
  void testDotMM();

  void testLUDec();


public:
  static UnitTest::TestSuite *suite();
};

}



#endif // LINALGTEST_HH
