#include "regression_test.hh"
#include "fluctuator.hh"
#include <sbml/SBMLTypes.h>



using namespace Fluc;


void
RegressionTest::testConstSpecies()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/const_species.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  UT_ASSERT_THROW(Models::REmodel(doc->getModel()),
                  Fluc::SBMLFeatureNotSupported);

  delete doc;
}


void
RegressionTest::testSpeciesByAssignmentRule()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/species_by_assignment_rule.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  UT_ASSERT_THROW(Models::REmodel(doc->getModel()),
                  Fluc::SBMLFeatureNotSupported);

  delete doc;
}


void
RegressionTest::testNonConstantParameter()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/non_const_parameter.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  UT_ASSERT_THROW(Models::REmodel(doc->getModel()),
                  Fluc::SBMLFeatureNotSupported);

  delete doc;
}


void
RegressionTest::testNonConstantCompartment()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/non_const_compartment.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  UT_ASSERT_THROW(Models::REmodel(doc->getModel()),
                  Fluc::SBMLFeatureNotSupported);

  delete doc;
}


void
RegressionTest::testAlgebraicConstraint()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/algebraicrules.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  UT_ASSERT_THROW(Models::REmodel(doc->getModel()),
                  Fluc::SBMLFeatureNotSupported);

  delete doc;
}


void
RegressionTest::testSpeciesByRateRule()
{
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile("test/regression-tests/rate_rules.xml");
  UT_ASSERT(0 == doc->getNumErrors());

  UT_ASSERT_THROW(Models::REmodel(doc->getModel()),
                  Fluc::SBMLFeatureNotSupported);

  delete doc;
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

