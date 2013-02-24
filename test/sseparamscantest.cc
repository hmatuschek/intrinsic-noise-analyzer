#include "sseparamscantest.hh"
#include <models/IOSmodel.hh>
#include <parser/sbml/sbml.hh>
#include <models/sseparamscan.hh>

using namespace iNA;


void
SSEParamScanTest::testEnzymeKinetics() {
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");

  // Prepare param-scan:
  Models::IOSmodel model(sbml_model);
  Models::ParameterScan<Models::IOSmodel> scan(model);
  std::vector<Models::ParameterSet> parameters(100);
  std::vector<Eigen::VectorXd> results(100);
  size_t N=10; double p_min=0.1, p_max=10.0;
  for (size_t i=0; i<N; i++) {
    parameters[i]["omega"] = p_min + i*((p_max-p_min)/N);
    results[i].resize(model.getDimension());
  }

  // Perform scan:
  scan.parameterScan(parameters, results);
}


UnitTest::TestSuite *
SSEParamScanTest::suite() {
  UnitTest::TestSuite *s = new UnitTest::TestSuite("SSE Parameter Scan Tests");

  s->addTest(new UnitTest::TestCaller<SSEParamScanTest>(
               "EnzymeKinetics Model", &SSEParamScanTest::testEnzymeKinetics));

  return s;
}
