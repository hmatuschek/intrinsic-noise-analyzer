#ifndef MODELCOPYTEST_HH
#define MODELCOPYTEST_HH

#include "unittest.hh"
#include "models/linearnoiseapproximation.hh"

namespace Fluc {

/**
 * Simple test to check if copying of models work.
 */
class ModelCopyTest : public UnitTest::TestCase
{
protected:
  void testVariableEqual(Ast::VariableDefinition *A, Ast::VariableDefinition *B, GiNaC::exmap &symbol_table);
  void testReactionEqual(Ast::Reaction *A, Ast::Reaction *b, GiNaC::exmap &symbol_table);

  void testModelEqual(Ast::Model &A, Ast::Model &B);
  void testScopeEqual(Ast::Scope &A, Ast::Scope &B, GiNaC::exmap &symbol_table);

  // Performs the test:
  void testCopy(const std::string &file);


public:
  /** Tests copying on ./test/regression-tests/coopkinetics1.xml. */
  void testCoopKinetics1();


public:
  static UnitTest::TestSuite *suite();
};

}

#endif // MODELCOPYTEST_HH
