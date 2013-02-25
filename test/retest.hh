#ifndef RETEST_HH
#define RETEST_HH

#include "unittest.hh"
#include <models/REmodel.hh>


namespace iNA {

/** Trivial test for the RE model & interpreter. */
class RETest : public UnitTest::TestCase
{
public:
  /** Using byte-code interpreter. */
  void testEnzymeKineticsBCI();
  /** Using JIT compiler. */
  void testEnzymeKineticsJIT();

public:
  /** Constructs the test-suite. */
  static UnitTest::TestSuite *suite();

private:
  /** Integrates the given model using the byte-code interpreter. */
  void integrateViaByteCode(Models::REmodel &model);
  /** Integrates the given model using the JIT compiler. */
  void integrateViaJIT(Models::REmodel &model);
};

}

#endif // RETEST_HH
