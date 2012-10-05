#ifndef CONSERVATIONANALYSISTEST_HH
#define CONSERVATIONANALYSISTEST_HH

#include "unittest.hh"
#include "ast/ast.hh"


namespace iNA {

class ConservationAnalysisTest : public UnitTest::TestCase
{
protected:
  Ast::Model *_model;

public:
  virtual ~ConservationAnalysisTest();

  void setUp();
  void tearDown();

  void testDirect();
  void testWithAssignmentRule();

public:
  static UnitTest::TestSuite *suite();
};


}

#endif // CONSERVATIONANALYSISTEST_HH
