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

private:
  /** Holds a weak reference to the list of reactions. */
  ReactionList *_reactions;
  /** The list of reactions. */
  QTableView *_reactionList;
  /** Shows the "new reaction" editor. */
  QPushButton *_newReacButton;
};

#endif // REACTIONLISTVIEW_HH
