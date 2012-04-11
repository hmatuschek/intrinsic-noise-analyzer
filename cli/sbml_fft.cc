#include "sbml_fft.hh"
//#include "models/unsupported.hh"
#include "math.hh"
#include <fstream>

using namespace Fluc;

int main(int argc, char *argv[])
{
  // Check args:
  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " SBML_FILENAME" << std::endl;
    return -1;
  }

  // Open document:
  libsbml::SBMLDocument *doc = libsbml::readSBMLFromFile(argv[1]);

  // Check for errors:
  if (0 < doc->getNumErrors())
  {
    std::cerr << "Error while reading SBML: ";
    doc->printErrors(std::cerr);
    return -1;
  }

  // Do the work:
  try
  {

    size_t steps = 500;

    double transientTime = 500;
    size_t realizations=10;
    // Construct SSA model from SBML model

    Models::LinearNoiseApproximation lna(doc->getModel());
    Models::SteadyStateAnalysis steadyState(lna);

    Models::SpectralAnalysis specA(lna);

    const double fMax   = 2*specA.getMaxFrequency(0.01)*10;//20*0.75/(3.14);
    const double deltaf = 0.0002;

    Models::SpectrumRecorder<Models::OptimizedSSA,libsbml::Model> specEval(doc->getModel(),realizations,fMax,deltaf);

    Eigen::VectorXd x;
    Eigen::MatrixXd LNAspec;

    Eigen::VectorXd freq = specEval.getFrequencies();

    Eigen::MatrixXd specCorr(lna.numSpecies(),freq.size());

    steadyState.calcSteadyState(x);
    steadyState.calcSpectrum(freq,LNAspec,false);
    specA.spectrum(freq,specCorr);

    specEval.advance(transientTime);
    std::cerr << "Transients passed..." << std::endl;

    std::ofstream myfile;
    Eigen::VectorXd spec;
    for(size_t m=0; m<steps; m++)
    {

        specEval.run();

        if(m%1==0){


        for(size_t specId=0; specId<lna.numSpecies();specId++)
        {
            std::stringstream fileName;
            fileName << "spectrum" << lna.getSpecies(specId)->getName() << ".dat";

            spec = specEval.getSpectrum(specId);

            // output spectrum

            myfile.open(fileName.str().c_str(),std::ios_base::trunc);
            for(int fs=0;fs<freq.size()/2;fs++)
            {
                myfile << freq(fs) << "\t"
                       << spec(fs) << "\t"
                       << LNAspec(fs,specId) << "\t"
                       << LNAspec(fs,specId)+specCorr(specId,fs) << "\n";
            }
            myfile.close();
        }

        }

    }

  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << err.str() << std::endl;
    return -1;
  }
}
