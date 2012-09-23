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
    reaction->getKineticLaw()->setRateLaw(forwardLaw);

    // Create a new irreversible backward reaction
    std::string id = reaction->getIdentifier();
    id=id.append("Reverse");

    std::string name = reaction->getName();
    name.append(" (Reverse)");

    // Create copy of kinetic law:
    GiNaC::exmap param_subst;
    Ast::KineticLaw *kineticLaw = Ast::Trafo::ModelCopyist::dupKineticLaw(reaction->getKineticLaw(), param_subst);
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


void IrreversibleReactionCollapsor::apply(Ast::Model &model)
{
  size_t count=0;

  // Iterate over all reactions:
  for (Ast::Model::ReactionIterator it=model.reactionsBegin(); it!=model.reactionsEnd(); it++)
  {
      Ast::Reaction *reaction1 = (*it);

    // Skip if reaction is reversible:
    if ( reaction1->isReversible()) { continue; }

    for (Ast::Model::ReactionIterator other=(it+1); other!=model.reactionsEnd(); other++)
    {

        Ast::Reaction *reaction2 = (*other);

        // Do comparison
        if(reaction1->isReverseOf(reaction2))
        {
            // Make reversible
            GiNaC::ex rateLaw = reaction1->getKineticLaw()->getRateLaw()-reaction2->getKineticLaw()->getRateLaw();
            reaction1->setReversible(true);
            reaction1->getKineticLaw()->setRateLaw(GiNaC::collect_common_factors(rateLaw));

            // and remove reverse reaction
            model.remDefinition(reaction2);
            std::cerr << "collapsed" << reaction1->getName() << " with \t" << reaction2->getName() << std::endl;

            // Create a log message.
            {
              Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
              message << "Collapsed reaction "<<reaction1->getName()<< " with" << reaction2->getName() <<".";
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
