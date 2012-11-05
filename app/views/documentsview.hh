#ifndef SBMLMODELSVIEW_HH
#define SBMLMODELSVIEW_HH

#include <QWidget>
#include <QTreeView>
#include <QAction>


/**
 * A simple QTreeView widget, that opens a new view custom a selected item.
 *
 * @ingroup gui
 */
class DocumentsView : public QTreeView
{
  Q_OBJECT

public:
  /**
   * Constructs an empty documents view.
   */
  explicit DocumentsView(QWidget *parent = 0);

protected slots:
  /**
   * When the selection of items in the model is changed, this method will signal the
   * Application instance, to show the corresponding panel in the main window.
   */
  void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

  /**
   * Handles a context-menu event.
   */
  void conextMenuRequested(const QPoint &pos);
};



/**
 * The Side-panel showing all open models and there analysises.
 *
 * @ingroup gui
 */
class DocumentsSidePanel : public QWidget
{
  Q_OBJECT

public:
  /**
   * Constructor.
   */
  explicit DocumentsSidePanel(QWidget *perent=0);

private:
  /**
   * The actual view.
   */
  DocumentsView *documents_view;

  /**
   * @deprecated Not used?
   */
  QAction *openDocumentAction;
};


#endif // SBMLMODELSVIEW_HH
