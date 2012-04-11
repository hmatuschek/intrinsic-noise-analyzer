#ifndef REACTIONVIEW_HH
#define REACTIONVIEW_HH

#include <QWidget>
#include "../doctree/reactionsitem.hh"


/**
 * Simple widget to display a reaction.
 *
 * The reaction will be shown as a title with the reaction name, the rendered reaction-equation
 * and a table of reaction-local parameters.
 *
 * @ingroup gui
 */
class ReactionView : public QWidget
{
  Q_OBJECT

public:
  /**
   * Constructs a view of the given @c ReactionItem.
   */
  explicit ReactionView(ReactionItem *reaction, QWidget *parent = 0);

private slots:
  /**
   * Gets called, if the reaction-item is deleted.
   */
  void reactionDestroyed();
};


#endif // REACTIONVIEW_HH
