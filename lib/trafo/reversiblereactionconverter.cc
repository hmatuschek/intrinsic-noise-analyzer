#include "reversiblereactionconverter.hh"
#include "ast/modelcopyist.hh"
#include "utils/logger.hh"


using namespace iNA;
using namespace iNA::Trafo;

void ReversibleReactionConverter::apply(Ast::Model &model)
{
  size_t count=0;

  // Iterate over all reactions:
  for (Ast::Model::ReactionIterator it=model.reactionsBegin(); it!=model.reactionsEnd(); it++)
  //for (size_t i=0; i<model.numReactions(); i++)
  {
      Ast::Reaction *reaction = (*it);

    // Check if reaction is reversible:
    if (! reaction->isReversible()) { continue; }

    GiNaC::ex numerator = reaction->getKineticLaw()->getRateLaw().numer().expand();
    GiNaC::ex denominator = reaction->getKineticLaw()->getRateLaw().denom();

    GiNaC::ex forwardLaw = numerator;
    GiNaC::ex reverseLaw = numerator;
    GiNaC::ex constantFlux = numerator;

    // swap reactant and products in reverse reaction
    for(Ast::Reaction::iterator reactant = reaction->reactantsBegin(); reactant!=reaction->reactantsEnd(); reactant++)
    {
      reverseLaw=reverseLaw.subs(reactant->first->getSymbol()==0);
      constantFlux=constantFlux.subs(reactant->first->getSymbol()==0);
    }
    for(Ast::Reaction::iterator product = reaction->productsBegin(); product!=reaction->productsEnd(); product++)
    {
      forwardLaw=forwardLaw.subs(product->first->getSymbol()==0);
      constantFlux=constantFlux.subs(product->first->getSymbol()==0);
    }

    // substract constant fluxes
    // from reactions with nonzero stoichiometries
    if(!constantFlux.is_zero()) {
      if(reaction->reactantsBegin()==reaction->reactantsEnd())
        reverseLaw-=constantFlux;
      else if(reaction->productsBegin()==reaction->productsEnd())
        forwardLaw-=constantFlux;
      else
        continue;
    }

    // Skip reaction if unsuccesful
    if (!numerator.is_equal(forwardLaw+reverseLaw)) {
      continue;
    }

    forwardLaw=forwardLaw/denominator;
    reverseLaw=-reverseLaw/denominator;

    // Set forward rate law
    reaction->setReversible(false);
    reaction->getKineticLaw()->setRateLaw(GiNaC::collect_common_factors(forwardLaw));

    // Create a new irreversible backward reaction
    std::string id = reaction->getIdentifier();
    id=id.append("Reverse");

    std::string name = reaction->getName();
    name.append(" (Reverse)");

    // Create copy of kinetic law:
    GiNaC::exmap param_subst;
    Ast::KineticLaw *kineticLaw = Ast::ModelCopyist::dupKineticLaw(reaction->getKineticLaw(), param_subst);
    // Substitute local paramters (if there are some) and make it look nice:
    kineticLaw->setRateLaw(GiNaC::collect_common_factors(reverseLaw.subs(param_subst)));
    // Assemble reverse reaction:
    Ast::Reaction *reverseReaction = new Ast::Reaction(id, name, kineticLaw, false);

    // swap reactant and products in reverse reaction
    for(Ast::Reaction::iterator species = reaction->reactantsBegin(); species!=reaction->reactantsEnd(); species++)
    {
      GiNaC::ex st = (reaction->getReactantStoichiometry(species->first->getSymbol()));
      reverseReaction->setProductStoichiometry( species->first,st );
    }
    for(Ast::Reaction::iterator species = reaction->productsBegin(); species!=reaction->productsEnd(); species++)
    {
      GiNaC::ex st = ( reaction->getProductStoichiometry(species->first->getSymbol()) );
      reverseReaction->setReactantStoichiometry( species->first,st );
    }

    // Clean up all redundant parameters
    reaction->getKineticLaw()->cleanUpParameters();
    reverseReaction->getKineticLaw()->cleanUpParameters();

    // Add reverse reaction after original one
    model.addDefinition(reverseReaction,reaction);

    count++;

    // Create a log message.
    {
      Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
      message << "Converted reversible reaction "<<reaction->getName()<<" to irreversible.";
      Utils::Logger::get().log(message);
    }
  }

  // Create final log message.
  if(count){
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Converted "<<count<<" reversible reactions succesfully.";
    Utils::Logger::get().log(message);
  }
}


void IrreversibleReactionCollapser::apply(Ast::Model &model)
{
  size_t count=0;

  // Iterate over all reactions:
  for (Ast::Model::ReactionIterator it=model.reactionsBegin(); it!=model.reactionsEnd(); it++)
  {
      Ast::Reaction *forward = (*it);

    // Skip if reaction is reversible:
    if ( forward->isReversible()) { continue; }

    for (Ast::Model::ReactionIterator other=(it+1); other!=model.reactionsEnd(); other++)
    {

        Ast::Reaction *reverse = (*other);

        // Do comparison
        if(forward->isReverseOf(reverse))
        {

            // Make reversible
            forward->setReversible(true);
            Ast::ModelCopyist::mergeReversibleKineticLaw(forward->getKineticLaw(),reverse->getKineticLaw());

            // and remove reverse reaction
            model.remDefinition(reverse);

            // Create a log message.
            {
              Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
              message << "Collapsed reaction "<<forward->getName()<< " with " << reverse->getName() <<".";
              Utils::Logger::get().log(message);
            }

            count++;
            break;
        }
    }

  }

  // Create final log message.
  if(count){
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Collapsed "<<count<<" reactions to reversible ones.";
    Utils::Logger::get().log(message);
  }
}
