#include "sbml_cle.hh"

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

    // Construct SSA model from SBML model
    Models::GillespieCLE model(doc->getModel(),1,1024);
    double dt=0.1;

    for(double t=0.; t<1000.; t+=dt)
    {
       Eigen::VectorXd state;
       Eigen::MatrixXd variance;
       model.stats(state,variance);
       std::cout <<t<< "\t" << state.transpose() << "\t" << variance.diagonal().transpose() << std::endl;
       model.run(dt);
    }

  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << err.str() << std::endl;
    return -1;
  }
}
