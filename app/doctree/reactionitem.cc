#include "reactionitem.hh"
#include "../views/reactionview.hh"



/* ********************************************************************************************* *
 * Implementation of ReactionWrapper a single reaction
 * ********************************************************************************************* */
ReactionItem::ReactionItem(iNA::Ast::Reaction *reaction, QObject *parent)
  : QObject(parent), _reaction(reaction), _local_parameters(0), _itemLabel()
{
  // Assemble display name
  _itemLabel = reaction->getLabel().c_str();
  // Create list model of local parameters:
  _local_parameters = new ReactionParameterList(reaction->getKineticLaw(), this);
}


const QString &
ReactionItem::getLabel() const {
  return this->_itemLabel;
}


QString
ReactionItem::getDisplayName() const {
  return this->_itemLabel;
}

void
ReactionItem::updateLabel() {
  _itemLabel = _reaction->getLabel().c_str();
}

iNA::Ast::Reaction *
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
