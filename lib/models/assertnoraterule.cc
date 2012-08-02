#include "assertnoraterule.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Models;


AssertNoRateRule::AssertNoRateRule(BaseModel &model)
{
  for (BaseModel::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    if (Ast::Node::isVariableDefinition(*iter))
    {
      Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*iter);

      if (var->hasRule() && Ast::Node::isRateRule(var->getRule()))
      {
        SBMLFeatureNotSupported err;
        err << "Rate-rules for variables not supported yet: "
            << " Variable " << var->getIdentifier() << " has a rate-rule.";
        throw err;
      }
    }
  }
}
