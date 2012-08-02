#include "assertconstantparametermixin.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Models;


AssertConstantParameterMixin::AssertConstantParameterMixin(BaseModel &model)
{
  for (BaseModel::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    if (Ast::Node::isParameter(*iter))
    {
      this->checkParameter(static_cast<Ast::Parameter *>(*iter));
    }

    if (Ast::Node::isReactionDefinition(*iter))
    {
      Ast::Reaction *reaction = static_cast<Ast::Reaction *>(*iter);

      // Check local parameters:
      for (Ast::KineticLaw::iterator liter = reaction->getKineticLaw()->begin();
           liter != reaction->getKineticLaw()->end(); liter++)
      {
        if (Ast::Node::isParameter(*liter))
        {
          this->checkParameter(static_cast<Ast::Parameter *>(*liter));
        }
      }
    }
  }
}


void
AssertConstantParameterMixin::checkParameter(Ast::Parameter *parameter)
{
  // Check if parameter is constant:
  if (! parameter->isConst())
  {
    SBMLFeatureNotSupported err;
    err << "This implementation can only handle constant paramters: "
        << "Parameter " << parameter->getIdentifier() << " is defined as non-constant.";
    throw err;
  }

  // Check if parameter has initial value:
  if (! parameter->hasValue())
  {
    SBMLFeatureNotSupported err;
    err << "Parameter " << parameter->getIdentifier() << " has no initial value assigned!";
    throw err;
  }
}
