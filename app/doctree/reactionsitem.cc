#include "reactionsitem.hh"
#include "../views/reactionlistview.hh"




/* ********************************************************************************************* *
 * Implementation of ReactionsWrapper (list of reactions)
 * ********************************************************************************************* */
ReactionsItem::ReactionsItem(Fluc::Ast::Model *model, QObject *parent) :
  QObject(parent), _itemLabel("Reactions"), _reactionList(model)
{
  // Populate reactions:
  for (size_t i=0; i<model->numReactions(); i++)
  {
    ReactionItem *reaction = new ReactionItem(model->getReaction(i), this);
    reaction->setTreeParent(this); this->_children.append(reaction);
  }

  // Done.
}


bool ReactionsItem::providesView() const { return true; }

QWidget *ReactionsItem::createView() { return new ReactionListView(this); }

const QString & ReactionsItem::getLabel() const { return _itemLabel; }

ReactionList *ReactionsItem::reactionList() { return &_reactionList; }





