#include "modelcopytest.hh"
#include "ast/ast.hh"

using namespace Fluc;



void
ModelCopyTest::testVariableEqual(Ast::VariableDefinition *A, Ast::VariableDefinition *B,
                                 GiNaC::exmap &symbol_table)
{
  // Check name:
  UT_ASSERT(A->hasName() == B->hasName());
  if (A->hasName()) {
    UT_ASSERT(A->getName() == B->getName());
  }

  // Check if variables has initial value:
  UT_ASSERT(A->hasValue() == B->hasValue());
  if (A->hasValue()) {
    A->getValue() == B->getValue().subs(symbol_table);
  }

  // Check if variables has rules:
  UT_ASSERT(A->hasRule() == A->hasRule());
  /// \todo Implement comparison of rules.
}


void
ModelCopyTest::testReactionEqual(Ast::Reaction *A, Ast::Reaction *B, GiNaC::exmap &symbol_table)
{
  // Check name
  UT_ASSERT(A->hasName() == B->hasName());
  if (A->hasName()) {
    UT_ASSERT(A->getName() == B->getName());
  }

  // Check reversible flag:
  UT_ASSERT(A->isReversible() == B->isReversible());

  /// \todo Test stoichiometry

  /// \todo Test kinetic law:
}



void
ModelCopyTest::testScopeEqual(Ast::Scope &A, Ast::Scope &B, GiNaC::exmap &symbol_table)
{
  // Iterate over all definitions in A:
  for (Ast::Scope::iterator item = A.begin(); item != A.end(); item++) {
    Ast::Definition *def_a = *item;
    // Check if model B has this definition:
    UT_ASSERT(B.hasDefinition(def_a->getIdentifier()));
    Ast::Definition *def_b = B.getDefinition(def_a->getIdentifier());
    // Check pointers:
    UT_ASSERT(def_a != def_b);
    // Check if both definitions have the same type-id:
    UT_ASSERT(def_a->getNodeType() == def_b->getNodeType());
    // Check if identifiers are identical
    UT_ASSERT(def_a->getIdentifier() == def_b->getIdentifier());

    if (Ast::Node::isVariableDefinition(def_a)) {
      Ast::VariableDefinition *var_a = dynamic_cast<Ast::VariableDefinition *>(def_a);
      Ast::VariableDefinition *var_b = dynamic_cast<Ast::VariableDefinition *>(def_b);
      // Check symbols
      UT_ASSERT(var_a->getSymbol() != var_b->getSymbol());
      // Populate symbol-table:
      symbol_table[var_b->getSymbol()] = var_a->getSymbol();
    }
  }
}



void
ModelCopyTest::testModelEqual(Ast::Model &A, Ast::Model &B)
{
  GiNaC::exmap symbol_table;
  // First perform basic checks and populate symbol-table:
  testScopeEqual(A,B, symbol_table);

  // Then, iterate over definitions in A and compare with definitions in B
  for (Ast::Model::iterator item=A.begin(); item != A.end(); item++)
  {
    Ast::Definition *def_a = *item;
    Ast::Definition *def_b = B.getDefinition(def_a->getIdentifier());

    if (Ast::Node::isVariableDefinition(*item)) {
      this->testVariableEqual(
            dynamic_cast<Ast::VariableDefinition *>(def_a),
            dynamic_cast<Ast::VariableDefinition *>(def_b), symbol_table);
    } else if (Ast::Node::isReactionDefinition(*item)) {
      this->testReactionEqual(
            dynamic_cast<Ast::Reaction *>(def_a),
            dynamic_cast<Ast::Reaction *>(def_b), symbol_table);
    }
  }
}



void
ModelCopyTest::testCopy(const std::string &file)
{
  // Load SBML...
  libsbml::SBMLDocument *document = libsbml::readSBMLFromFile(file.c_str());
  UT_ASSERT(0 == document->getNumErrors());

  // Construct model from SBML
  Ast::Model modelA(document->getModel());
  Ast::Model modelB(modelA);  // Copy model

  testModelEqual(modelA, modelB);
  testModelEqual(modelB, modelA);

  delete document;
}



void
ModelCopyTest::testCoopKinetics1()
{
  this->testCopy("test/regression-tests/coopkinetics1.xml");
}


UnitTest::TestSuite *
ModelCopyTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Model Copy Tests");

  s->addTest(new UnitTest::TestCaller<ModelCopyTest>(
               "coopkinetics1.xml", &ModelCopyTest::testCoopKinetics1));

  return s;
}
