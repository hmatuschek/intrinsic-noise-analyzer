#include "assertnoassignmentrule.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


AssertNoAssignmentRule::AssertNoAssignmentRule(BaseModel &model)
{
  for (BaseModel::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    if (Ast::Node::isVariableDefinition(*iter))
    {
      Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*iter);

      if (var->hasRule() && Ast::Node::isAssignmentRule(var->getRule()))
      {
        SBMLFeatureNotSupported err;
        err << "Assignment rules for variables not supported yet: "
            << " Variable " << var->getIdentifier() << " has an assignment rule.";
        throw err;
      }
    }
  }
}
