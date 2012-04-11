#include "assertirreversiblereactions.hh"

using namespace Fluc;
using namespace Fluc::Models;


AssertIrreversibleReactions::AssertIrreversibleReactions(BaseModel &model)
{
  // Iterate over all reactions:
  for (BaseModel::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    // Skip all non-reaction definitions:
    if (! Ast::Node::isReactionDefinition(*iter))
      continue;

    // Check if reaction is reversible:
    if (static_cast<Ast::Reaction *>(*iter)->isReversible())
    {
      SBMLFeatureNotSupported err;
      err << "Reversible reactions are not supported yet: Reaction "
          << (*iter)->getIdentifier() << " is defined to be reversible!";
      throw err;
    }
  }
}
