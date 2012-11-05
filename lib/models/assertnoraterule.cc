#include "assertnoraterule.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


AssertNoRateRule::AssertNoRateRule(BaseModel &model)
{
  custom (BaseModel::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    if (Ast::Node::isVariableDefinition(*iter))
    {
      Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*iter);

      if (var->hasRule() && Ast::Node::isRateRule(var->getRule()))
      {
        SBMLFeatureNotSupported err;
        err << "Rate-rules custom variables not supported yet: "
            << " Variable " << var->getIdentifier() << " has a rate-rule.";
        throw err;
      }
    }
  }
}
