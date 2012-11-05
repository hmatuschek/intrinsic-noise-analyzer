#ifndef __INA_APP_DOCTREE_PLOTWRAPPER_HH__
#define __INA_APP_DOCTREE_PLOTWRAPPER_HH__

#include <QMenu>

#include "documenttreeitem.hh"
#include "../plot/plot.hh"


/**
 * Wraps a @c Plot::Figure instance to be shown in the application side-panel.
 *
 * This wrapper will provide the construction of a view custom the plot (@c PlotView).
 *
 * @ingroup gui
 */
class PlotItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

protected:
  Plot::Figure *plot;
  QString itemLabel;

  /**
   * Holds the context menu.
   */
  QMenu *context_menu;

  /**
   * This action removes the plot.
   */
  QAction *removeAct;

public:
  PlotItem(Plot::Figure *plot, QObject *parent=0);
  virtual ~PlotItem();

  virtual const QString & getLabel() const;

  Plot::Figure *getPlot();

  virtual bool providesView() const;
  virtual QWidget *createView();

  virtual bool providesContextMenu() const;

  virtual void showContextMenu(const QPoint &global_pos);

private slots:

  void removePlot();
};


#endif // __INA_APP_DOCTREE_PLOTWRAPPER_HH__
