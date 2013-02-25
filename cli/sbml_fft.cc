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

  iNA::Models::LNAmodel mod(sbml_model);

  iNA::Models::LNASpec LNA(mod);


  // Do the work:


    size_t steps = 5000;

    double transientTime = 20;//goodwin 50;//brussel500;
    size_t realizations=4;
    // Construct SSA model from SBML model

    const double fMax   = 10;//goodwin 10*50/(2*3.14); //brusselator20*0.75/(3.14);
    const double deltaf = 0.005;

    Models::SpectrumRecorder<Models::OptimizedSSA> specEval(sbml_model,realizations,fMax,deltaf,realizations);

    Eigen::VectorXd x;

    Eigen::VectorXd freq = specEval.getFrequencies();


    // Calc LNA
    Eigen::MatrixXd LNAspec = LNA.getSpectrum(freq);


    specEval.advance(transientTime);
    std::cerr << "Transients passed..." << std::endl;

    std::ofstream myfile;
    Eigen::VectorXd spec;
    for(size_t m=0; m<steps; m++)
    {

        specEval.run();

        if(m%1==0){


        for(size_t idx=0; idx<sbml_model.numSpecies();idx++)
        {
            std::stringstream fileName;
            fileName << "spectrum" << sbml_model.getSpecies(idx)->getIdentifier() << ".dat";

            spec = specEval.getSpectrum(idx);

            // output spectrum
            myfile.open(fileName.str().c_str(),std::ios_base::trunc);
            for(int fs=0;fs<freq.size()/2;fs++)
            {
                myfile << freq(fs) << "\t"
                       << spec(fs) << "\t"
                       << LNAspec(fs,idx) << "\n";
            }
            myfile.close();

        }

        }


        std::cout << (m+1) << "/" << steps << std::endl;

    }


}
