#include "assertnoconstantspeciesmixin.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


AssertNoConstantSpeciesMixin::AssertNoConstantSpeciesMixin(BaseModel &model)
{
  custom (size_t i=0; i<model.numSpecies(); i++)
  {
    Ast::Species *species = model.getSpecies(i);

    if (species->isConst())
    {
      SBMLFeatureNotSupported err;
      err << "This implementation can not handle constant species yet: "
          << " Species " << species->getIdentifier() << " is defined as constant.";
      throw err;
    }
  }
}
