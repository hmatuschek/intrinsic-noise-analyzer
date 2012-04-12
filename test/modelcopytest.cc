#include "modelcopytest.hh"
#include "ast/ast.hh"

using namespace Fluc;




void
ModelCopyTest::testCopy(const std::string &file)
{
  // Load SBML...
  libsbml::SBMLDocument *document = libsbml::readSBMLFromFile(file.c_str());
  UT_ASSERT(0 != document);
  // Construct model from SBML
  Ast::Model modelA(document->getModel());
  Ast::Model modelB(modelA);  // Copy model
  // will hold symbol table to translate symbols of model B -> symbols of model A.
  GiNaC::exmap symbol_table;

  // Iterate over all definitions of model A
  for (Ast::Model::iterator def = modelA.begin(); def != modelA.end(); def++) {
    Ast::Definition *def_a = *def;
    // Check if model B has this definition:
    UT_ASSERT(modelB.hasDefinition(def_a->getIdentifier()));
    Ast::Definition *def_b = modelB.getDefinition(def_a->getIdentifier());
    // Check if both definitions have the same type-id:
    UT_ASSERT(def_a->getNodeType() == def_b->getNodeType());
    // Check if identifiers are identical
    UT_ASSERT(def_a->getIdentifier() == def_b->getIdentifier());

    if (Ast::Node::isVariableDefinition(*def)) {
      // Check variable definition:
      Ast::VariableDefinition *var_def_a = dynamic_cast<Ast::VariableDefinition *>(def_a);
      Ast::VariableDefinition *var_def_b = dynamic_cast<Ast::VariableDefinition *>(def_b);
      // Check if copy is also a variable definition:
      UT_ASSERT(0 != var_def_b);
      // Check name:
      UT_ASSERT(var_def_a->hasName() == var_def_b->hasName());
      if (var_def_a->hasName()) {
        UT_ASSERT(var_def_a->getName() == var_def_b->getName());
      }
      // Check if variables has initial value:
      UT_ASSERT(var_def_a->hasValue() == var_def_b->hasValue());
      // Check if variables has rules:
      UT_ASSERT(var_def_a->hasRule() == var_def_b->hasRule());
      // Populate symbol table:
      symbol_table[var_def_b->getSymbol()] = var_def_a->getSymbol();

    } else if (Ast::Node::isReactionDefinition(*def)) {
      // Check reaction definitions
      Ast::Reaction *reac_a = dynamic_cast<Ast::Reaction *>(def_a);
      Ast::Reaction *reac_b = dynamic_cast<Ast::Reaction *>(def_b);
      // Check type:
      UT_ASSERT(0 != reac_b);
      // Check name
      UT_ASSERT(reac_a->hasName() == reac_b->hasName());
      if (reac_a->hasName()) {
        UT_ASSERT(reac_a->getName() == reac_b->getName());
      }
      // Check reversible flag:
      UT_ASSERT(reac_a->isReversible() == reac_b->isReversible());
    }
  }
}


void
ModelCopyTest::testCoopKinetics1()
{
  this->testCopy("./test/regression-tests/coopkinetics1.xml");
}


UnitTest::TestSuite *
ModelCopyTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Model Copy Tests");

  s->addTest(new UnitTest::TestCaller<ModelCopyTest>(
               "coopkinetics1.xml", &ModelCopyTest::testCoopKinetics1));

  return s;
}
