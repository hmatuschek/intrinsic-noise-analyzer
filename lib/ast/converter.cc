#include "converter.hh"
#include "trafo/modelcopyist.hh"


using namespace Fluc;
using namespace Fluc::Ast;

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

      for(size_t i=0; i<model.numSpecies(); i++)
      {

          if ( reaction->hasReactant( model.getSpecies(i)->getSymbol()) )
          {
              backwardLaw=backwardLaw.subs(model.getSpecies(i)->getSymbol()==0);
          }
          if ( reaction->hasProduct(model.getSpecies(i)->getSymbol()) )
          {
              forwardLaw=forwardLaw.subs(model.getSpecies(i)->getSymbol()==0);
          }
      }

      // skip reaction if unsuccesful
      if (!numerator.is_equal(forwardLaw+backwardLaw))
      {
            continue;
//          SBMLFeatureNotSupported err;
//          err << "Reversible reaction "
//              << (*iter)->getIdentifier() << " could not be converted to a reversible one!";
//          throw err;
      }

      forwardLaw=forwardLaw/denominator;
      backwardLaw=-backwardLaw/denominator;


      std::cerr<<reaction->getName()<<std::endl;

      std::cerr<<"numerator: "<<numerator<<std::endl;
      std::cerr<<"forward: "<<forwardLaw<<std::endl;
      std::cerr<<"backward: "<<backwardLaw<<std::endl;

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
      Ast::KineticLaw *kineticLaw = Ast::Trafo::ModelCopyist::dupKineticLaw(reaction->getKineticLaw(), param_subst);
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

      std::cerr<<"success"<<std::endl;

    }
  }

  if(count) std::cerr<<"Converted "<<count<<" reactions succesfully."<<std::endl;

}
