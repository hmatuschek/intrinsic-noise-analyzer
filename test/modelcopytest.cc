#include "modelcopytest.hh"
#include "ast/ast.hh"

using namespace Fluc;



void
ModelCopyTest::testRuleEqual(Ast::Rule *A, Ast::Rule *B,
                             GiNaC::exmap &symbol_table)
{
  // Check pointer:
  UT_ASSERT(A != B);

  // Check node-type:
  UT_ASSERT(A->getNodeType() == A->getNodeType());

  // Check rule-expression:
  UT_ASSERT(A->getRule() == B->getRule().subs(symbol_table));
}


void
ModelCopyTest::testParameterEqual(Ast::Parameter *A, Ast::Parameter *B,
                                  GiNaC::exmap &symbol_table)
{
  // Pass...
}


void
ModelCopyTest::testSpeciesEqual(Ast::Species *A, Ast::Species *B,
                                GiNaC::exmap &symbol_table)
{
  // Check compartments
  testVariableEqual(A->getCompartment(), B->getCompartment(), symbol_table);
}


void
ModelCopyTest::testCompartmentEqual(Ast::Compartment *A, Ast::Compartment *B,
                                    GiNaC::exmap &symbol_table)
{
  // Pass...
}


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

  // Check if variable is constant.
  UT_ASSERT(A->isConst() == B->isConst());

  // Compare units:
  UT_ASSERT(A->getUnit() == B->getUnit());

  // Check if variables has rules:
  UT_ASSERT(A->hasRule() == A->hasRule());
  if (A->hasRule()) {
    testRuleEqual(A->getRule(), B->getRule(), symbol_table);
  }

  if (Ast::Node::isSpecies(A)) {
    testSpeciesEqual(
          dynamic_cast<Ast::Species *>(A),
          dynamic_cast<Ast::Species *>(B),
          symbol_table);
  } else if (Ast::Node::isParameter(A)) {
    testParameterEqual(
          dynamic_cast<Ast::Parameter *>(A),
          dynamic_cast<Ast::Parameter *>(B),
          symbol_table);
  } else if (Ast::Node::isCompartment(A)) {
    testCompartmentEqual(
          dynamic_cast<Ast::Compartment *>(A),
          dynamic_cast<Ast::Compartment *>(B),
          symbol_table);
  }
}


void
ModelCopyTest::testKineticLaw(Ast::KineticLaw *A, Ast::KineticLaw *B, GiNaC::exmap &symbol_table)
{
  // Strore copy of symbol_table
  GiNaC::exmap temp_table(symbol_table);

  // Process local parameters:
  testScopeEqual(*A, *B, temp_table);

  // compare law:
  UT_ASSERT(A->getRateLaw() == B->getRateLaw().subs(temp_table));
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

  // Test reactant stoichiometry
  for (Ast::Reaction::iterator reac = B->reacBegin(); reac != B->reacEnd(); reac++) {
    GiNaC::symbol reac_sym = GiNaC::ex_to<GiNaC::symbol>(symbol_table[reac->first->getSymbol()]);
    // Check if reactant is in A:
    UT_ASSERT(A->hasReactant(reac_sym));
    // Check stoichiometry:
    UT_ASSERT(reac->second == A->getReactantStoichiometry(reac_sym).subs(symbol_table));
  }

  // Test product stoichiometry
  for (Ast::Reaction::iterator prod = B->prodBegin(); prod != B->prodEnd(); prod++) {
    GiNaC::symbol prod_sym = GiNaC::ex_to<GiNaC::symbol>(symbol_table[prod->first->getSymbol()]);
    // Check if product is in A:
    UT_ASSERT(A->hasProduct(prod_sym));
    // Check stoichiometry:
    UT_ASSERT(prod->second == A->getProductStoichiometry(prod_sym).subs(symbol_table));
  }

  // Test modifier.
  for (Ast::Reaction::mod_iterator mod = B->modBegin(); mod != B->modEnd(); mod++) {
    GiNaC::symbol mod_sym = GiNaC::ex_to<GiNaC::symbol>(symbol_table[(*mod)->getSymbol()]);
    // Check if A has modifier:
    UT_ASSERT(A->isModifier(mod_sym));
  }

  // Test kinetic law:
  testKineticLaw(A->getKineticLaw(), B->getKineticLaw(), symbol_table);
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