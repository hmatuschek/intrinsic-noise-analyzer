#include "assertnoconstantspeciesmixin.hh"

using namespace Fluc;
using namespace Fluc::Models;


AssertNoConstantSpeciesMixin::AssertNoConstantSpeciesMixin(BaseModel &model)
{
  for (size_t i=0; i<model.numSpecies(); i++)
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
