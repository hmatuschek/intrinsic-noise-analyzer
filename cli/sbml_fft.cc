#include "sbml_fft.hh"
#include "sbml_lna.hh"
#include <parser/sbml/sbml.hh>
#include "math.hh"
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

  // Do the work:
  try
  {

    size_t steps = 500;

    double transientTime = 500;
    size_t realizations=4;
    // Construct SSA model from SBML model

    const double fMax   = 5;//20*0.75/(3.14);
    const double deltaf = 0.001;

    Models::SpectrumRecorder<Models::OptimizedSSA> specEval(sbml_model,realizations,fMax,deltaf,realizations);

    Eigen::VectorXd x;

    Eigen::VectorXd freq = specEval.getFrequencies();

    specEval.advance(transientTime);
    std::cerr << "Transients passed..." << std::endl;

    std::ofstream myfile;
    Eigen::VectorXd spec;
    for(size_t m=0; m<steps; m++)
    {

        specEval.run();

        if(m%1==0){


        for(size_t specId=0; specId<sbml_model.numSpecies();specId++)
        {
            std::stringstream fileName;
            fileName << "spectrum" << sbml_model.getSpecies(specId)->getName() << ".dat";

            spec = specEval.getSpectrum(specId);

            // output spectrum
            myfile.open(fileName.str().c_str(),std::ios_base::trunc);
            for(int fs=0;fs<freq.size()/2;fs++)
            {
                myfile << freq(fs) << "\t"
                       << spec(fs) << "\n";
            }
            myfile.close();

        }

        }


        std::cout << (m+1) << "/" << steps << std::endl;

    }

  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA. " << std::endl;
    return -1;
  }
}
