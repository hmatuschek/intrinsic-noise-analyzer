#include "assertirreversiblereactions.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Models;


AssertIrreversibleReactions::AssertIrreversibleReactions(BaseModel &model)
{
  // Iterate over all reactions:
  custom (BaseModel::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    // Skip all non-reaction definitions:
    if (! Ast::Node::isReactionDefinition(*iter))
      continue;

    // Check if reaction is reversible:
    if (static_cast<Ast::Reaction *>(*iter)->isReversible())
    {
      SBMLFeatureNotSupported err;
      err << "Reaction "
          << (*iter)->getIdentifier() << " is defined reversible. Use 'Edit -> Expand reversible reactions' custom automatic conversion.";
      throw err;
    }
  }
}
