#include "mersennetwistertest.hh"
#include <cmath>

using namespace iNA;


#include "mersennetwister.hh"


void
MersenneTwisterTest::compareRNG()
{
  MersenneTwister      rng_a(1234);
  MersenneTwister      rng_b(1234);

  for (size_t i=0; i<1024; i++) {
    // Compare random numbers:
    UT_ASSERT_EQUAL(rng_a.rand(), rng_b.rand());
  }

  rng_a.seed(1234);
  rng_b.seed(5678);
  for (size_t i=0; i<1024; i++) {
    // Compare random numbers:
    UT_ASSERT(rng_a.rand() != rng_b.rand());
  }
}



void
MersenneTwisterTest::testCummulative()
{
  const size_t N=100;
  const size_t M=1024*1024;

  std::vector<double> counts_a(N, 0.0);
  std::vector<double> counts_b(N, 0.0);

  srand(1234);
  MersenneTwister   rng_b(1234);

  // Performs Monte-Carlo integration of f(x)=x:
  for(size_t i=0; i<M; i++) {
    double val_a = double(rand())/(RAND_MAX);
    double val_b = rng_b.rand();

    for (size_t j=0; j<N; j++) {
      if (val_a < double(j+1)/N) {
        counts_a[j]++;
      }
      if (val_b < double(j+1)/N) {
        counts_b[j]++;
      }
    }
  }

  // Check:
  for (size_t j=0; j<N; j++) {
    UT_ASSERT(std::fabs((counts_a[j]/M)-(double(j+1)/N)) <= (double(N*N)/M));
    UT_ASSERT(std::fabs((counts_b[j]/M)-(double(j+1)/N)) <= (double(N*N)/M));
  }
}


UnitTest::TestSuite *
MersenneTwisterTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("RNG test");

  s->addTest(new UnitTest::TestCaller<MersenneTwisterTest>(
               "Direct comparison", &MersenneTwisterTest::compareRNG));

  s->addTest(new UnitTest::TestCaller<MersenneTwisterTest>(
               "Test cummulative distribution", &MersenneTwisterTest::testCummulative));

  return s;
}
