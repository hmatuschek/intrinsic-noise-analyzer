#include "makeparamglobal.hh"

#include <ast/reaction.hh>
#include <exception.hh>

using namespace iNA;
using namespace iNA::Trafo;


void
iNA::Trafo::makeParameterGlobal(const std::string &param, const std::string &reac, Ast::Model &model)
{
  // Find reaction:
  Ast::Reaction *reaction = model.getReaction(reac);
  // Find parameter:
  Ast::Parameter *parameter = reaction->getKineticLaw()->getParameter(param);
  // Get a new identifier for the paramter in the global scope:
  std::string new_id = model.getNewIdentifier(parameter->getIdentifier());
  // Set new identifier if needed
  if (new_id != parameter->getIdentifier()) {
    reaction->getKineticLaw()->resetIdentifier(parameter->getIdentifier(), new_id);
  }
  // Remove parameter from kinetic law / reaction
  reaction->getKineticLaw()->remDefinition(parameter);
  // and add it to the model (global scope)
  model.addDefinition(parameter);
}
