#include "assertnonempty.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


AssertNonEmpty::AssertNonEmpty(BaseModel &model)
{

  if (!model.numCompartments())
  {
    SBMLFeatureNotSupported err;
    err << "The model defines no compartments.";
    throw err;
  }

  if (!model.numSpecies())
  {
    SBMLFeatureNotSupported err;
    err << "The model defines no species.";
    throw err;
  }

  if (!model.numReactions())
  {
    SBMLFeatureNotSupported err;
    err << "The model defines no reactions.";
    throw err;
  }

}
