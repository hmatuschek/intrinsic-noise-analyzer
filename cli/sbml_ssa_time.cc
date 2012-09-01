#include "sbml_ssa_time.hh"

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
    Models::OptimizedSSA model(doc->getModel(),1,1024,2,1);
    double dt=100;

    for(double t=0.; t<dt; t+=dt)
    {
       //model.stats(state,variance);
       model.run(dt);
    }

  }
  catch (Exception err)
  {
    std::cerr << "Can not perform SSA: " << err.str() << std::endl;
    return -1;
  }
}
