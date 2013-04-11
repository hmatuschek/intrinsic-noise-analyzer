#ifndef __INA_APP_DOCTREE_REACTIONSITEM_HH__
#define __INA_APP_DOCTREE_REACTIONSITEM_HH__

#include <QObject>
#include "ast/model.hh"
#include "reactionitem.hh"
#include "documenttreeitem.hh"
#include "../models/reactionlist.hh"


/** A trivial wrapper around a list model of all reactions defined in this model. */
class ReactionsItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ReactionsItem(iNA::Ast::Model *model, QObject *parent = 0);

  /** Returns true. */
  virtual bool providesView() const;
  /** Creates an ReactionListView instance. */
  virtual QWidget *createView();

  /** Returns the label. */
  virtual const QString &getLabel() const;

  /** Returns a weak reference to the list of reactions. */
  ReactionList *reactionList();

  /** Removes the specifies item from the list of reactions, also modifies the underlaying
   * @c iNA::Ast::Model and frees the item and reaction instance. */
  virtual void removeChild(TreeItem *node);

protected:
  /** Updates the reaction list (children) from model. */
  virtual void resetTree();

private:
  /** Holds a weak reference to the model instance. */
  iNA::Ast::Model *_model;
  /** Holds the item label ("Reactions"). */
  QString _itemLabel;
  /** Holds the list model of all reactions. */
  ReactionList _reactionList;
};


#endif // __INA_APP_DOCTREE_REACTIONSITEM_HH__
