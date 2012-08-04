#include "reactionitem.hh"
#include "../views/reactionview.hh"



/* ********************************************************************************************* *
 * Implementation of ReactionWrapper a single reaction
 * ********************************************************************************************* */
ReactionItem::ReactionItem(Fluc::Ast::Reaction *reaction, Fluc::Ast::Model *model, QObject *parent)
  : QObject(parent), _reaction(reaction), _model(model), local_parameters(0), itemLabel()
{
  // Assemble display name
  if (reaction->hasName()){
    itemLabel = reaction->getName().c_str();
  } else {
    itemLabel = reaction->getIdentifier().c_str();
  }

  // Create list model of local parameters:
  this->local_parameters = new ReactionParameterList(reaction->getKineticLaw(), _model, this);
}


const QString &
ReactionItem::getLabel() const
{
  return this->itemLabel;
}


QString
ReactionItem::getDisplayName() const
{
  return this->itemLabel;
}


Fluc::Ast::Reaction *
ReactionItem::getReaction()
{
  return this->_reaction;
}


ReactionParameterList *
ReactionItem::localParameters()
{
  return this->local_parameters;
}


bool
ReactionItem::providesView() const
{
  return true;
}


QWidget *
ReactionItem::createView()
{
  return new ReactionView(this);
}
