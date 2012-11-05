#include "mathtest.hh"
#include <math.h>
#include "math.hh"


void
iNA::MathTest::testErf()
{
  size_t N = 10000;
  double x_max = 1e6;
  double x_min = -1e6;
  double dx = (x_max-x_min)/N;
  double x = x_min;

  // Sample intervall:
  for (size_t i=0; i<N; i++)
  {
    UT_ASSERT_NEAR(erf(x), iNA::erf(x)); x += dx;
  }
}


iNA::UnitTest::TestSuite *
iNA::MathTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests for mathematical utilities.");
  s->addTest(new UnitTest::TestCaller<MathTest>("erf()",
                                                &MathTest::testErf));

  return s;
}
