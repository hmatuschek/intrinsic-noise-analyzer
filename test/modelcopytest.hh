#ifndef MODELCOPYTEST_HH
#define MODELCOPYTEST_HH

#include "unittest.hh"
#include "ast/ast.hh"

namespace iNA {

/**
 * Simple test to check if copying of models work.
 */
class ModelCopyTest : public UnitTest::TestCase
{
protected:
  void testConstraint(Ast::Constraint *A, Ast::Constraint *B, GiNaC::exmap &symbol_table);
  void testAlgebraicConstraint(Ast::AlgebraicConstraint *A, Ast::AlgebraicConstraint *B,
                               GiNaC::exmap &symbol_table);

  void testFunctionDefinition(Ast::FunctionDefinition *A, Ast::FunctionDefinition *B);

  void testVariableEqual(Ast::VariableDefinition *A, Ast::VariableDefinition *B, GiNaC::exmap &symbol_table);
  void testRuleEqual(Ast::Rule *A, Ast::Rule *B, GiNaC::exmap &symbol_table);
  void testParameterEqual(Ast::Parameter *A, Ast::Parameter *B, GiNaC::exmap &symbol_table);
  void testSpeciesEqual(Ast::Species *A, Ast::Species *B, GiNaC::exmap &symbol_table);
  void testCompartmentEqual(Ast::Compartment *A, Ast::Compartment *B, GiNaC::exmap &symbol_table);

  void testReactionEqual(Ast::Reaction *A, Ast::Reaction *B, GiNaC::exmap &symbol_table);
  void testKineticLaw(Ast::KineticLaw *A, Ast::KineticLaw *B, GiNaC::exmap &symbol_table);

  void testModelEqual(Ast::Model &A, Ast::Model &B);
  void testScopeEqual(Ast::Scope &A, Ast::Scope &B, GiNaC::exmap &symbol_table);

  // Performs the test:
  void testCopy(const std::string &file);


public:
  virtual ~ModelCopyTest()
  {};
  /** Tests copying on ./test/regression-tests/coopkinetics1.xml. */
  void testCoopKinetics1();


public:
  static UnitTest::TestSuite *suite();
};

}

#endif // MODELCOPYTEST_HH
