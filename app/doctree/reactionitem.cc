#include "reactionitem.hh"
#include "../views/reactionview.hh"



/* ********************************************************************************************* *
 * Implementation of ReactionWrapper a single reaction
 * ********************************************************************************************* */
ReactionItem::ReactionItem(Fluc::Ast::Reaction *reaction, QObject *parent)
  : QObject(parent), reaction(reaction), local_parameters(0), itemLabel()
{
  if (reaction->hasName())
  {
    itemLabel = reaction->getName().c_str();
  }
  else
  {
    itemLabel = reaction->getIdentifier().c_str();
  }

  this->local_parameters = new ReactionParameterList(reaction->getKineticLaw(), this);
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
  return this->reaction;
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
