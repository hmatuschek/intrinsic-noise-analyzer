#include "conservationanalysistest.hh"

#include "parser/parser.hh"
#include "trafo/trafo.hh"


using namespace Fluc;

ConservationAnalysisTest::~ConservationAnalysisTest()
{}

void
ConservationAnalysisTest::setUp()
{
  std::stringstream text;

  text << "@model:3.3.1 = enzymatic \"Enzymatic Reaction\"" << std::endl
       << std::endl
       << "@compartments" << std::endl
       << "  cytosol = 1e5 \"Cytosol\"" << std::endl
       << std::endl
       << "@species" << std::endl
       << "  cytosol: [ES] = 0  \"ES\"" << std::endl
       << "  cytosol: [P]  = 0  \"P\"" << std::endl
       << "  cytosol: [S]  = 10 \"S\"" << std::endl
       << "  cytosol: [E]  = 10 \"E\"" << std::endl
       << std::endl
       << "@reactions" << std::endl
       << "  @r = veq \"veq\"" << std::endl
       << "    E + S -> ES" << std::endl
       << "    cytosol*(k1*E*S - k2*ES): k1=1e6, k2=0.2" << std::endl
       << "  @r = vcat \"vcat\"" << std::endl
       << "    ES -> E + P" << std::endl
       << "    cytosol*k1*ES: k1=1" << std::endl;   //

  // parse model
  _model = Parser::Sbmlsh::importModel(text);
}

void
ConservationAnalysisTest::tearDown()
{
  delete _model;
}


void
ConservationAnalysisTest::testDirect()
{
  Trafo::ConservationAnalysis::apply(*_model);
  // Finds:
  //  S = - ES - P
  //  E = - ES
  GiNaC::ex E  = _model->getSpecies("E")->getSymbol();
  GiNaC::ex S  = _model->getSpecies("S")->getSymbol();
  GiNaC::ex ES = _model->getSpecies("ES")->getSymbol();
  GiNaC::ex P  = _model->getSpecies("P")->getSymbol();

  // Check if species S has an assignment rule S = -ES - P
  UT_ASSERT(_model->getSpecies("S")->hasRule());
  UT_ASSERT(Ast::Node::isAssignmentRule(_model->getSpecies("S")->getRule()));
  UT_ASSERT_EQUAL(_model->getSpecies("S")->getRule()->getRule(), -ES-P);

  // Check if species E has assignment rule E = -ES:
  UT_ASSERT(_model->getSpecies("E")->hasRule());
  UT_ASSERT(Ast::Node::isAssignmentRule(_model->getSpecies("E")->getRule()));
  UT_ASSERT_EQUAL(_model->getSpecies("E")->getRule()->getRule(), -ES);
}


void
ConservationAnalysisTest::testWithAssignmentRule()
{
  GiNaC::ex E  = _model->getSpecies("E")->getSymbol();
  GiNaC::ex S  = _model->getSpecies("S")->getSymbol();
  GiNaC::ex ES = _model->getSpecies("ES")->getSymbol();
  GiNaC::ex P  = _model->getSpecies("P")->getSymbol();

  // Define a conserved cycle explicitly as a assignment rule: S == -ES - P
  _model->getSpecies("S")->setRule(new Ast::AssignmentRule(-ES-P));

  // now perform conservation analysis on remainting independent species:
  Trafo::ConservationAnalysis::apply(*_model);

  // Should find conserved cycle E = ES;
  UT_ASSERT(_model->getSpecies("E")->hasRule());
  UT_ASSERT(Ast::Node::isAssignmentRule(_model->getSpecies("E")->getRule()));
  UT_ASSERT_EQUAL(_model->getSpecies("E")->getRule()->getRule(), -ES);
}


void
ConservationAnalysisTest::testWithAlgebraicConstraint()
{
  GiNaC::ex E  = _model->getSpecies("E")->getSymbol();
  GiNaC::ex S  = _model->getSpecies("S")->getSymbol();
  GiNaC::ex ES = _model->getSpecies("ES")->getSymbol();
  GiNaC::ex P  = _model->getSpecies("P")->getSymbol();

  // Define a conserved cycle explicitly as algebraic constraint: 0 == ES + P + S
  _model->addConstraint(new Ast::AlgebraicConstraint(ES+P+S));

  // Resolve algebraic constraints as assignment rules: in this case ES = -P - S
  Trafo::AlgebraicConstraintSolver::apply(*_model);

  // now perform conservation analysis on remainting independent species:
  Trafo::ConservationAnalysis::apply(*_model);

  // Should find conserved cycle E = P + S;
  UT_ASSERT(_model->getSpecies("E")->hasRule());
  UT_ASSERT(Ast::Node::isAssignmentRule(_model->getSpecies("E")->getRule()));
  UT_ASSERT_EQUAL(_model->getSpecies("E")->getRule()->getRule(), P+S);
}



UnitTest::TestSuite *
ConservationAnalysisTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Tests Trafo::ConservationAnalysis");

  s->addTest(new UnitTest::TestCaller<ConservationAnalysisTest>(
               "simple analysis", &ConservationAnalysisTest::testDirect));

  s->addTest(new UnitTest::TestCaller<ConservationAnalysisTest>(
               "with assignment rules", &ConservationAnalysisTest::testWithAssignmentRule));

  s->addTest(new UnitTest::TestCaller<ConservationAnalysisTest>(
               "with algebraic constraints", &ConservationAnalysisTest::testWithAlgebraicConstraint));

  return s;
}
