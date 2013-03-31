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
    Models::GenericOptimizedSSA< Eval::jit::Engine<Eigen::VectorXd> > model(sbml_model, 1, std::time(0));

    double dt=0.1;

    double tMax = 1e10;

    std::cout << "Start SSA" << std::endl;
    model.run(50);
    std::cout << "Transients passed..." << std::endl;


    std::vector<Models::Histogram<double> > hist(model.numSpecies());
    std::ofstream histfile;

    int step=0;

    for(double t=0.; t<tMax; t+=dt, step++)
    {

       if (step%(10)==0) std::cout << "t=" << t <<std::endl;

       model.run(dt);

       for(size_t idx=0; idx<model.numSpecies(); idx++)
       {

           model.getHistogram(idx,hist[idx]);

           // output histogram
           std::stringstream fileName;
           fileName << "histogram" << sbml_model.getSpecies(idx)->getIdentifier() << ".dat";
           histfile.open(fileName.str().c_str(),std::ios_base::trunc);

           std::map<double,double> temp = hist[idx].getDensity();
           for(std::map<double,double>::iterator it=temp.begin();it!=temp.end();it++)
               histfile << it->first << "\t" << it->second << "\t"<<std::endl;
           histfile.close();

       }
    }


    std::cout << "Finished!" << std::endl;


}
