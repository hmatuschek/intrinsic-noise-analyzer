#include "sbml_pscan.hh"
#include "../unsupported/unsupported.hh"
#include <parser/sbml/sbml.hh>

using namespace iNA;


int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  double tfinal = 10;
  double dt = 0.1;
  double epsilon_abs=1.e-9;
  double epsilon_rel=1.e-5;
  size_t threads = 2;

  size_t steps=10;

    // Construct LNA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);

    Models::IOSmodel model(sbml_model);
    Models::TimeCourseParameterScan<Models::IOSmodel> pscan(model);

    // Choose parameter
    Ast::Parameter* param = model.getParameter("feedback");

    // Make list with values
    std::vector<iNA::Models::ParameterSet> parameters(steps);

    double minVal = 0.;
    double maxVal = 1.;
    double dx = (maxVal - minVal)/(steps);
    for(size_t j=0; j<steps; j++)
    {
        double val = minVal + dx*(j);
        parameters[j].insert(std::pair<std::string,double>(param->getIdentifier(),val));
    }

    // perform the parameter scan
    std::vector<Eigen::VectorXd> result;
    pscan.parameterScan(parameters,tfinal,dt,epsilon_abs,epsilon_rel,threads);




}
