#include "sseparamscantest.hh"
#include <models/IOSmodel.hh>
#include <parser/sbml/sbml.hh>
#include <models/sseparamscan.hh>

using namespace iNA;


SSEParamScanTest::~SSEParamScanTest() {
  // pass...
}

void
SSEParamScanTest::testEnzymeKinetics() {
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/enzymekinetics1.xml");

  size_t N=100;
  // Prepare param-scan:
  Models::IOSmodel model(sbml_model);
  Models::ParameterScan<Models::IOSmodel> scan(model);
  std::vector<Models::ParameterSet> parameters(N);
  std::vector<Eigen::VectorXd> results(N);
  double p_min=0.1, p_max=10.0;
  for (size_t i=0; i<N; i++) {
    parameters[i]["omega"] = p_min + i*((p_max-p_min)/N);
    results[i].resize(model.getDimension());
  }

  // Perform scan:
  scan.parameterScan(parameters, results);

  bool success = true;
  for (size_t i=0; i<N; i++) {
    if (!results[i].allFinite()) {
      std::cerr << "Solution of SteadyStateSolver of paramset (" << i
                << ") contains NaNs: " << results[i].transpose() << std::endl;
      success = false;
    }
  }
  UT_ASSERT(success);
}


void
SSEParamScanTest::testGene1() {
  // Read doc and check for errors:
  Ast::Model sbml_model;
  Parser::Sbml::importModel(sbml_model, "test/regression-tests/gene1.xml");

  size_t N=100;
  // Prepare param-scan:
  Models::REmodel model(sbml_model);
  Models::ParameterScan<Models::REmodel> scan(model);
  std::vector<Models::ParameterSet> parameters(N);
  std::vector<Eigen::VectorXd> results(N);
  double p_min=1445, p_max=1590;
  for (size_t i=0; i<N; i++) {
    parameters[i]["k0"] = p_min + i*((p_max-p_min)/N);
    results[i].resize(model.getDimension());
  }

  // Perform scan:
  scan.parameterScan(parameters, results);
  bool success = true;
  for (size_t i=0; i<N; i++) {
    if (!results[i].allFinite()) {
      std::cerr << "Solution of SteadyStateSolver of paramset (" << i
                << ") contains NaNs: " << results[i].transpose() << std::endl;
      success = false;
    }
  }
  UT_ASSERT(success);
}


UnitTest::TestSuite *
SSEParamScanTest::suite() {
  UnitTest::TestSuite *s = new UnitTest::TestSuite("SSE Parameter Scan Tests");

  s->addTest(new UnitTest::TestCaller<SSEParamScanTest>(
               "EnzymeKinetics Model", &SSEParamScanTest::testEnzymeKinetics));

  s->addTest(new UnitTest::TestCaller<SSEParamScanTest>(
               "Gene Model 1 (RE)", &SSEParamScanTest::testGene1));

  return s;
}
