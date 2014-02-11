#include "reactionsitem.hh"
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../views/reactionlistview.hh"




/* ********************************************************************************************* *
 * Implementation of ReactionsItem (list of reactions)
 * ********************************************************************************************* */
ReactionsItem::ReactionsItem(iNA::Ast::Model *model, QObject *parent) :
  QObject(parent), _model(model), _itemLabel("Reactions"), _reactionList(model)
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

ReactionList *
ReactionsItem::reactionList() {
  return &_reactionList;
}

void
ReactionsItem::addReaction(ReactionItem *reaction_item) {
  _model->addDefinition(reaction_item->getReaction());
  addChild(reaction_item);
}

void
ReactionsItem::removeChild(TreeItem *item) {
  ReactionItem *reaction_item = 0;
  if (0 == (reaction_item = dynamic_cast<ReactionItem *>(item))) { return; }
  iNA::Ast::Reaction *reaction = reaction_item->getReaction();
  _model->remDefinition(reaction);
  DocumentTreeItem::removeChild(item);
  delete reaction; delete reaction_item;
}


void
ReactionsItem::updateReactionList() {
  // Clear reaction list:
  for (QList<TreeItem *>::iterator item=_children.begin(); item!=_children.end(); item++) {
    delete static_cast<ReactionItem *>(*item);
  }
  _children.clear();

  // Populate reactions:
  for (size_t i=0; i<_reactionList.getModel().numReactions(); i++)
  {
    ReactionItem *reaction = new ReactionItem(_reactionList.getModel().getReaction(i), this);
    reaction->setTreeParent(this); this->_children.append(reaction);
  }

  // update reaction list table model:
  _reactionList.updateTable();

}

void
ReactionsItem::resetTree() {
  updateReactionList();
}


