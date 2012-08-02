#include "sbml_ssa.hh"
#include <parser/sbml/sbml.hh>


using namespace Fluc;

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  // Do the work:
  try
  {

    // Construct SSA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);
    Models::OptimizedSSA model(sbml_model, 30, 1024);
    double dt=0.1;

    Eigen::VectorXd state;
    Eigen::MatrixXd variance;

    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());

    for(double t=0.; t<10.; t+=dt)
    {

       //model.stats(state,variance,skewness);
       std::cout <<t<< "\t" << state.transpose() << "\t";
       std::cout << variance.diagonal().transpose() << "\t";

       std::cout<<std::endl;
       model.run(dt);
    }

  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << std::endl;
    return -1;
  }
}
