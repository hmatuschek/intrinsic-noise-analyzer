#include "converter.hh"
#include "modelcopyist.hh"
#include "utils/logger.hh"


using namespace iNA;
using namespace iNA::Ast;

Convert2Irreversible::Convert2Irreversible(Ast::Model &model)
    : model(model)

{

}


void Convert2Irreversible::process()
{
  size_t count=0;
  // Iterate over all reactions:
  for (Ast::Model::iterator iter = model.begin(); iter != model.end(); iter++)
  {
    // Skip all non-reaction definitions:
    if (! Ast::Node::isReactionDefinition(*iter))
      continue;

    Ast::Reaction *reaction= static_cast<Ast::Reaction *>(*iter);

    // Check if reaction is reversible:
    if (reaction->isReversible())
    {

      GiNaC::ex numerator = reaction->getKineticLaw()->getRateLaw().numer().expand();
      GiNaC::ex denominator = reaction->getKineticLaw()->getRateLaw().denom();

      GiNaC::ex forwardLaw = numerator;
      GiNaC::ex backwardLaw = numerator;
      GiNaC::ex constantFlux = numerator;

      // swap reactant and products in reverse reaction
      for(Ast::Reaction::iterator reactant = reaction->reacBegin(); reactant!=reaction->reacEnd(); reactant++)
      {
          backwardLaw=backwardLaw.subs(reactant->first->getSymbol()==0);
          constantFlux=constantFlux.subs(reactant->first->getSymbol()==0);
      }
      for(Ast::Reaction::iterator product = reaction->prodBegin(); product!=reaction->prodEnd(); product++)
      {
          forwardLaw=forwardLaw.subs(product->first->getSymbol()==0);
          constantFlux=constantFlux.subs(product->first->getSymbol()==0);
      }

      // substract constant fluxes
      // from reactions with nonzero stoichiometries
      if(!constantFlux.is_zero())
      {
          if(reaction->reacBegin()==reaction->reacEnd())
              backwardLaw-=constantFlux;
          else if(reaction->prodBegin()==reaction->prodEnd())
              forwardLaw-=constantFlux;
          else
              continue;
      }


      // skip reaction if unsuccesful
      if (!numerator.is_equal(forwardLaw+backwardLaw))
      {
            continue;
      }

      forwardLaw=forwardLaw/denominator;
      backwardLaw=-backwardLaw/denominator;

      // set forward rate law
      reaction->setReversible(false);
      reaction->getKineticLaw()->setRateLaw(forwardLaw);

      // create a new irreversible backward reaction
      std::string id = reaction->getIdentifier();
      id=id.append("Reverse");

      std::string name = reaction->getName();
      name.append(" (Reverse)");

      // Create copy of kinetic law:
      GiNaC::exmap param_subst;
      Ast::KineticLaw *kineticLaw = Ast::ModelCopyist::dupKineticLaw(reaction->getKineticLaw(), param_subst);
      // Substitute local paramters (if there are some):
      kineticLaw->setRateLaw(backwardLaw.subs(param_subst));
      // Assemble reverse reaction:
      Ast::Reaction *backwardReaction = new Ast::Reaction(id, name, kineticLaw, false);


      // swap reactant and products in reverse reaction
      for(Ast::Reaction::iterator species = reaction->reacBegin(); species!=reaction->reacEnd(); species++)
      {
              GiNaC::ex st = (reaction->getReactantStoichiometry(species->first->getSymbol()));
              backwardReaction->setProductStoichiometry( species->first,st );
      }
      for(Ast::Reaction::iterator species = reaction->prodBegin(); species!=reaction->prodEnd(); species++)
      {
              GiNaC::ex st = ( reaction->getProductStoichiometry(species->first->getSymbol()) );
              backwardReaction->setReactantStoichiometry( species->first,st );
      }

      // add backward reaction
      model.addDefinition(backwardReaction);

      count++;

      {
        Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
        message << "Converted reversible reaction "<<reaction->getName()<<" to irreversible.";
        Utils::Logger::get().log(message);
      }

    }
  }

  if(count){
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Converted "<<count<<" reversible reactions succesfully.";
    Utils::Logger::get().log(message);
  }

}
