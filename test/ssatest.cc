#include "ssatest.hh"
#include <parser/sbml/sbml.hh>
#include <models/optimizedSSA.hh>


using namespace iNA;

void
SSATest::testEnzymeKinetics()
{
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");

  // Assemble and perform small SSA:
  Models::OptimizedSSA ssa(sbml_model, 2, 1234);

  // Perform analysis:
  size_t N = 100; double t_end = 1.0; double dt = t_end/N;
  for (size_t i=0; i<N; i++) { ssa.run(dt); }
}



UnitTest::TestSuite *
SSATest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("Stochastic Simulation Tests");

  s->addTest(new UnitTest::TestCaller<SSATest>(
               "EnzymeKinetics Model", &SSATest::testEnzymeKinetics));

  return s;
}
