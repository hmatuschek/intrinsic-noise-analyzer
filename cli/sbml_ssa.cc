#include "sbml_ssa.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>
#include "unsupported.hh"

using namespace iNA;

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
    Models::OptimizedSSA model(sbml_model, 1, time(0));
    double dt=0.1;

    Models::Histogram<double> hist;

    std::cout << "Start SSA" << std::endl;
    model.run(100);
    std::cout << "Transients passed..." << std::endl;

    Eigen::VectorXd mean(model.numSpecies());

    Eigen::VectorXd skew(model.numSpecies());
    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());


    std::ofstream outfile;

    outfile.open ("traj.dat");

    for(double t=0.; t<1000.; t+=dt)
    {

       //std::cout << "t=" << t <<std::endl;

       model.run(dt);
       model.stats(mean,cov,skew);

       outfile << t;
       for(int i=0; i<model.numSpecies(); i++)
            outfile << "\t" << mean(i);
       outfile<<std::endl;

    }


    outfile.close();


  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << std::endl;
    return -1;
  }
}
