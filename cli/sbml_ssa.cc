#include "sbml_ssa.hh"
#include <parser/sbml/sbml.hh>
#include <fstream>

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
    Models::OptimizedSSA model(sbml_model, 30, 1024);
    double dt=0.1;

    Eigen::VectorXd mean;
    Eigen::MatrixXd variance;

    Eigen::MatrixXd cov(model.numSpecies(),model.numSpecies());

    //std::map<double,double> hist;

    Models::Histogram<double> hist;

    std::ofstream histfile;
    model.run(1);
    for(double t=0.; t<10.; t+=dt)
    {

       model.run(dt);
       model.getHistogram(2,hist);

       histfile.open ("histogram.dat");
       std::map<double,double> temp=hist.getNormalized();
       for(std::map<double,double>::iterator it=temp.begin();it!=temp.end();it++)
           histfile << it->first << "\t" << it->second << "\t"<<std::endl;
       histfile.close();

    }


  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << std::endl;
    return -1;
  }
}
