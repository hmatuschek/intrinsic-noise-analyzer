#include "steadystatetest.hh"
#include <models/REmodel.hh>
#include <models/LNAmodel.hh>
#include <models/IOSmodel.hh>
#include <models/sseinterpreter.hh>
#include <models/steadystateanalysis.hh>
#include <eval/jit/engine.hh>
#include <parser/sbml/sbml.hh>


using namespace iNA;


SteadyStateTest::~SteadyStateTest() {
  // pass...
}


void
SteadyStateTest::testEnzymeKineticsRE() {
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/extended_goodwin.xml");
  // Construct RE model to integrate
  Models::REmodel model(sbml_model);
  // Perform analysis:
  Eigen::VectorXd state(model.getDimension());
  Models::SteadyStateAnalysis<Models::REmodel> analysis(model);
  analysis.setMaxIterations(1000);
  analysis.calcSteadyState(state);
}

void
SteadyStateTest::testEnzymeKineticsLNA() {
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/extended_goodwin.xml");
  // Construct LNA model to integrate
  Models::LNAmodel model(sbml_model);
  // Perform analysis:
  Eigen::VectorXd state(model.getDimension());
  Models::SteadyStateAnalysis<Models::LNAmodel> analysis(model);
  analysis.setMaxIterations(1000);
  analysis.calcSteadyState(state);
}

void
SteadyStateTest::testEnzymeKineticsIOS() {
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/extended_goodwin.xml");
  // Construct model to integrate
  Models::IOSmodel model(sbml_model);
  // Perform analysis:
  Eigen::VectorXd state(model.getDimension());
  Models::SteadyStateAnalysis<Models::IOSmodel> analysis(model);
  analysis.setMaxIterations(1000);
  analysis.calcSteadyState(state);
}


UnitTest::TestSuite *
SteadyStateTest::suite() {
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Steady State Tests");

  s->addTest(new UnitTest::TestCaller<SteadyStateTest>(
               "EnzymeKinetics Model (RE)", &SteadyStateTest::testEnzymeKineticsRE));

  s->addTest(new UnitTest::TestCaller<SteadyStateTest>(
               "EnzymeKinetics Model (LNA)", &SteadyStateTest::testEnzymeKineticsLNA));

  s->addTest(new UnitTest::TestCaller<SteadyStateTest>(
               "EnzymeKinetics Model (IOS)", &SteadyStateTest::testEnzymeKineticsIOS));

  return s;
}
