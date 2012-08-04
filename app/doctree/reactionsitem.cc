#include "reactionsitem.hh"
#include "../views/reactionview.hh"




/* ********************************************************************************************* *
 * Implementation of ReactionsWrapper (list of reactions)
 * ********************************************************************************************* */
ReactionsItem::ReactionsItem(Fluc::Ast::Model *model, QObject *parent) :
  QObject(parent), itemLabel("Reactions")
{
  // Populate reactions:
  for (size_t i=0; i<model->numReactions(); i++)
  {
    ReactionItem *reaction = new ReactionItem(model->getReaction(i), model, this);
    reaction->setTreeParent(this); this->_children.append(reaction);
  }

  // Done.
}


const QString &
ReactionsItem::getLabel() const
{
  return this->itemLabel;
}






