#ifndef REACTIONVIEW_HH
#define REACTIONVIEW_HH

#include <QWidget>
#include <QItemSelection>
#include <QPushButton>
#include <QTableView>

#include <QGraphicsView>
#include "../doctree/reactionsitem.hh"

class ReactionGraphic
        : public QGraphicsView
{

    Q_OBJECT

public:
    ReactionGraphic()
        : QGraphicsView()
    {
        // Pass...
    }

protected:
    void mouseDoubleClickEvent(QMouseEvent *event)
    {
        emit this->doubleClicked();
    }

signals:
    void doubleClicked();

};

/** Simple widget to display a reaction.
 * The reaction will be shown as a title with the reaction name, the rendered reaction-equation
 * and a table of reaction-local parameters. */
class ReactionView : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a view of the given @c ReactionItem. */
  explicit ReactionView(ReactionItem *reaction, QWidget *parent = 0);

private slots:
  /** Will be called if "add param" is clicked. */
  void onAddParamClicked();
  /** Will be called if "rem param" is clicked. */
  void onRemParamClicked();
  /** Will be called if the selection of the parameterlist is changed, is used to enable/disable
   * the "rem param" button. */
  void onSelectionChanged(const QItemSelection &selected,const QItemSelection &unselected);
  /** Will be called for double click on kinetic law. */
  void onReactionEditing();
private:
  /** Holds a weak reference to the reaction item being shown. */
  ReactionItem *_reaction;
  /** The "add param" button. */
  QPushButton *_addParamButton;
  /** The "rem param" button. */
  QPushButton *_remParamButton;
  /** Holds the table view of local parameters. */
  QTableView *_paramTable;
};


#endif // REACTIONVIEW_HH
