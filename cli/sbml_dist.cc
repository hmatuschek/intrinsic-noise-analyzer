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

    // Construct SSA model from SBML model
    Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);
    Models::OptimizedSSA model(sbml_model, 30, 1024,1);

    double dt=1;


    int idx = 1;
    double tMax = 10000;

    std::cout << "Selected: " << sbml_model.getSpecies(idx)->getIdentifier() << std::endl;

    std::cout << "Start SSA" << std::endl;
    model.run(30);
    std::cout << "Transients passed..." << std::endl;


    Models::Histogram<double> hist;
    std::ofstream histfile;

    for(double t=0.; t<tMax; t+=dt)
    {

      //if (t%(tMax/10)) std::cout << "t=" << t <<std::endl;

       model.run(dt);
       model.getHistogram(idx,hist);

       histfile.open ("histogram.dat");
       std::map<double,double> temp=hist.getDensity();
       for(std::map<double,double>::iterator it=temp.begin();it!=temp.end();it++)
           histfile << it->first << "\t" << it->second << "\t"<<std::endl;
       histfile.close();

    }


    std::cout << "Finished!" << std::endl;


}
