#include "reactionitem.hh"
#include "../views/reactionview.hh"



/* ********************************************************************************************* *
 * Implementation of ReactionWrapper a single reaction
 * ********************************************************************************************* */
ReactionItem::ReactionItem(Fluc::Ast::Reaction *reaction, QObject *parent)
  : QObject(parent), _reaction(reaction), _local_parameters(0), _itemLabel()
{
  // Assemble display name
  if (reaction->hasName()){
    _itemLabel = reaction->getName().c_str();
  } else {
    _itemLabel = reaction->getIdentifier().c_str();
  }

  // Create list model of local parameters:
  _local_parameters = new ReactionParameterList(reaction->getKineticLaw(), this);
}


const QString &
ReactionItem::getLabel() const
{
  return this->_itemLabel;
}


QString
ReactionItem::getDisplayName() const
{
  return this->_itemLabel;
}


Fluc::Ast::Reaction *
ReactionItem::getReaction()
{
  return this->_reaction;
}


ReactionParameterList *
ReactionItem::localParameters()
{
  return _local_parameters;
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
