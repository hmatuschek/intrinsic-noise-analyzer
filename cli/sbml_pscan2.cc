#include "sbml_pscan2.hh"
#include "../unsupported/unsupported.hh"
#include <parser/sbml/sbml.hh>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <string>
#include <sstream>
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

  // Construct LNA model from SBML model
  Ast::Model sbml_model; Parser::Sbml::importModel(sbml_model, argv[1]);

      double transients = 8;

      size_t steps= 100;

      double min = 10;
      double max = 0;
      double maxtime = 1.e100;
      double timestep =0.1;

      double interval = (max-min)/steps;

      std::string id = "feedback";



    // Construct parameter sets
    std::vector<iNA::Models::ParameterSet> _parameterSets(steps+1);
    for(size_t j = 0; j<=steps; j++)
    {
      double val = min+interval*j;
      _parameterSets[j].insert(
            std::pair<std::string,double>(id,std::exp(-val)));
    }

    iNA::Models::ParamScanInterface *_pscan=0;

    // Construct analysis:
    _pscan = new iNA::Models::SSAparamScan< iNA::Eval::jit::Engine<Eigen::VectorXd> >
                 (sbml_model,
                  _parameterSets, transients, OpenMP::getMaxThreads(),2);

    size_t _Ns = sbml_model.numSpecies();

    {
      std::ofstream file;
      file.open ("header.dat");

      // label parameter
      file << id << "\t";

      // label of means
      for (size_t i=0; i<_Ns; i++)
        file << sbml_model.getSpecies(i)->getIdentifier() << "\t";

      // label covariances
      //int idx = 0;
      for (size_t i=0; i<_Ns; i++)
        for (size_t j=0; j<=i; j++)
          file << "COV(" << sbml_model.getSpecies(i)->getIdentifier() << "," << sbml_model.getSpecies(i)->getIdentifier() << ")\t";

      file << std::endl;

      file.close();
    }

    // Iterate
    int _current_iteration = 0;
    double _simulation_time=0.;
    for (; _simulation_time<=maxtime; _simulation_time+=timestep)
    {
      // perform a step:
      _pscan->run(timestep);
      _current_iteration++;

      // skip output
      if(!_current_iteration%1000) continue;
        std::cout<<_current_iteration<<std::endl;

      std::ofstream file;
      file.open ("out.dat");

      for(size_t pid=0; pid<_parameterSets.size(); pid++)
      {
        file << GiNaC::ex_to<GiNaC::numeric>(
                  _parameterSets[pid][id]).to_double() << "\t";

        //int col=1;

        // output means
        for (size_t i=0; i<_Ns; i++)
          file << _pscan->getMean()(pid,i) << "\t";

        // output covariances
        int idx = 0;
        for (size_t i=0; i<_Ns; i++)
          for (size_t j=0; j<=i; j++)
            file <<  _pscan->getCovariance()(pid,idx++) << "\t";

        file << std::endl;
      }

      file.close();

    }



}
