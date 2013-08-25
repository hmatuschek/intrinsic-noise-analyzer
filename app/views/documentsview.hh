#ifndef __INA_APP_VIEWS_DOCUMENTSVIEW_HH__
#define __INA_APP_VIEWS_DOCUMENTSVIEW_HH__

#include <QWidget>
#include <QTreeView>
#include <QAction>


/**
 * A simple QTreeView widget, that opens a new view for a selected item.
 * @ingroup gui
 */
class DocumentsView : public QTreeView
{
  Q_OBJECT

public:
  /** Constructs an empty documents view. */
  explicit DocumentsView(QWidget *parent = 0);

protected slots:
  /** When the selection of items in the model is changed, this method will signal the
   * Application instance, to show the corresponding panel in the main window. */
  void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

  /** Handles a context-menu event. */
  void conextMenuRequested(const QPoint &pos);

  /** Gets called whenever an item requests to be selected (and visible) */
  void onItemAutoView(const QModelIndex &item);

protected:
  /** Implements the DragEnter event handler. */
  virtual void dragEnterEvent(QDragEnterEvent *event);
  /** Implements the DragMove event handler. */
  virtual void dragMoveEvent(QDragMoveEvent *event);
  /** Implements the DragLeave event handler. */
  virtual void dragLeaveEvent(QDragLeaveEvent *event);
  /** Implements the DropEvent event handler. */
  virtual void dropEvent(QDropEvent *event);
};



#endif // __INA_APP_VIEWS_DOCUMENTSVIEW_HH__
