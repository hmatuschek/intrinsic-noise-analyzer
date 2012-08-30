#include "sbml_pscan.hh"
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

  // Construct LNA model from SBML model
  Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);

    Models::LNAmodel model(sbml_model);
    Models::SteadyStateAnalysis<Models::LNAmodel> steadyState(model);

    // choose parameter
    Ast::Parameter* param = model.getReaction(0)->getKineticLaw()->getParameter(0);

    // make list with values
    std::vector<GiNaC::exmap> parameters(10);

    double minVal = 0.1e-5;
    double maxVal = 2.e-5;
    double dx = (maxVal - minVal)/9.;
    for(int j=0; j<10; j++)
    {
        double val = minVal + dx*(j);
        parameters[j].insert(std::pair<GiNaC::ex,GiNaC::ex>(param->getSymbol(),val));
    }



    // perform the parameter scan
    std::vector<Eigen::VectorXd> result;
    steadyState.parameterScan(parameters,result);

    Eigen::VectorXd concentrations(model.numSpecies());
    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());
    Eigen::VectorXd emre(model.numSpecies());

    // table header
    std::cout << param->getIdentifier() << "\t";
    for(size_t i=0; i<model.numSpecies(); i++)
    {
            std::cout<< model.getSpecies(i)->getName() <<"\t";
    }

    std::cout<<std::endl;

    for(size_t j=0; j<result.size();j++)
    {

        std::cout << (*parameters[j].find(param->getSymbol())).second << "\t";
        model.fullState(result[j],concentrations,cov,emre);
        for(size_t i=0; i<model.numSpecies(); i++)
        {
            std::cout << concentrations(i) <<"\t";
        }

        std::cout<<std::endl;

    }



}
