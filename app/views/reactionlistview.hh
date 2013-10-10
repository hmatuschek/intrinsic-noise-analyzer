#ifndef __INA_APP_VIEWS_REACTIONLISTVIEW_HH__
#define __INA_APP_VIEWS_REACTIONLISTVIEW_HH__

#include <QWidget>
#include <QTableView>
#include <QPushButton>

#include "../doctree/reactionsitem.hh"
#include "../models/reactionlist.hh"


/** Tiny class to list all reactions in a compact way. */
class ReactionListView : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ReactionListView(ReactionsItem *reactions, QWidget *parent = 0);


private slots:
  /** Callback for the "new reaction" button. */
  void onNewReaction();
  /** Callback to remove the selected reaction. */
  void onRemReaction();
  /** Enables or disabled the "delete reaction" button. */
  void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deseleced);
  /** Callback for double click on a reaction. */
  void onReactionEditing(const QModelIndex &index);
  /** Gets called if the model was modified. */
  void onModelModified();

private:
  /** Holds a weak reference to the list of reactions. */
  ReactionList *_reactions;
  /** Holds a weak reference to the ReactionsItem instance. */
  ReactionsItem *_reactions_item;
  /** The list of reactions. */
  QTableView *_reactionList;
  /** Shows the "new reaction" editor. */
  QPushButton *_newReacButton;
  /** Removes the selected reaction. */
  QPushButton *_remReacButton;
};

#endif // REACTIONLISTVIEW_HH
