#include "regression_test.hh"
#include "ina.hh"


using namespace Fluc;


void
RegressionTest::testConstSpecies()
{
  // load model from SBML file
  Ast::Model *sbml_model = Parser::Sbml::importModel("test/regression-tests/const_species.xml");

  Ast::Species *S1 = sbml_model->getSpecies("S1");
  UT_ASSERT(S1->isConst());

  UT_ASSERT_THROW(Models::REmodel re_model(*sbml_model),
                  Fluc::SBMLFeatureNotSupported);

  delete sbml_model;
}


void
RegressionTest::testSpeciesByAssignmentRule()
{
  Ast::Model *sbml_model = Parser::Sbml::importModel("test/regression-tests/species_by_assignment_rule.xml");

  Ast::Species *S3 = sbml_model->getSpecies("S3");
  UT_ASSERT(S3->hasRule());

  UT_ASSERT_THROW(Models::REmodel re_model(*sbml_model),
                  Fluc::SBMLFeatureNotSupported);

  delete sbml_model;
}


void
RegressionTest::testNonConstantParameter()
{
  Ast::Model *sbml_model = Parser::Sbml::importModel("test/regression-tests/non_const_parameter.xml");

  UT_ASSERT_THROW(Models::REmodel re_model(*sbml_model),
                  Fluc::SBMLFeatureNotSupported);

  delete sbml_model;
}


void
RegressionTest::testNonConstantCompartment()
{
  Ast::Model *sbml_model = Parser::Sbml::importModel("test/regression-tests/non_const_compartment.xml");

  UT_ASSERT_THROW(Models::REmodel re_model(*sbml_model),
                  Fluc::SBMLFeatureNotSupported);

  delete sbml_model;
}


void
RegressionTest::testAlgebraicConstraint()
{
  Ast::Model *model = Parser::Sbml::importModel("test/regression-tests/algebraicrules.xml");

  UT_ASSERT_THROW(Models::REmodel re_model(*model),
                  Fluc::SBMLFeatureNotSupported);

  delete model;
}


void
RegressionTest::testSpeciesByRateRule()
{
  Ast::Model *model = Parser::Sbml::importModel("test/regression-tests/rate_rules.xml");

  UT_ASSERT_THROW(Models::REmodel re_model(*model),
                  Fluc::SBMLFeatureNotSupported);

  delete model;
}


UnitTest::TestSuite *
RegressionTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Regression Tests");

  s->addTest(new UnitTest::TestCaller<RegressionTest>(
               "Constant species definition.", &RegressionTest::testConstSpecies));
  s->addTest(new UnitTest::TestCaller<RegressionTest>(
               "Species by assignment rule.", &RegressionTest::testSpeciesByAssignmentRule));
  s->addTest(new UnitTest::TestCaller<RegressionTest>(
               "Algebraic constraints.", &RegressionTest::testAlgebraicConstraint));
  s->addTest(new UnitTest::TestCaller<RegressionTest>(
               "Non-constant parameters.", &RegressionTest::testNonConstantParameter));
  s->addTest(new UnitTest::TestCaller<RegressionTest>(
               "Non-constant compartment.", &RegressionTest::testNonConstantCompartment));
  s->addTest(new UnitTest::TestCaller<RegressionTest>(
               "Species by rate-rule.", &RegressionTest::testSpeciesByRateRule));

  return s;
}

