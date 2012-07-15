#include "sbml_fsp.hh"
#include "models/unsupported/fsp.hh"

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

  Models::FSP fsp(doc->getModel());

}
