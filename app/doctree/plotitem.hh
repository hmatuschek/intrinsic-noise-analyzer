#ifndef __INA_APP_DOCTREE_PLOTWRAPPER_HH__
#define __INA_APP_DOCTREE_PLOTWRAPPER_HH__

#include <QMenu>

#include "documenttreeitem.hh"

// Forward declarations
namespace Plot {
class Figure;
class PlotConfig;
}

/** Wraps a @c Plot::Figure instance to be shown in the application side-panel.
 * This wrapper will provide the construction of a view for the plot (@c PlotView).
 * @ingroup gui
 */
class PlotItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

public:
  PlotItem(Plot::Figure *_plot, QObject *parent=0);
  PlotItem(Plot::PlotConfig *config, QObject *parent=0);
  virtual ~PlotItem();

  virtual const QString & getLabel() const;

  Plot::Figure *getPlot();
  bool hasConfig();
  Plot::PlotConfig *config();
  void updatePlot();

  virtual bool providesView() const;
  virtual QWidget *createView();

  virtual bool providesContextMenu() const;

  virtual void showContextMenu(const QPoint &global_pos);

private slots:
  void removePlot();

private:
  /** Holds the figure instance, might be created by the config object. */
  Plot::Figure *_plot;
  /** Holds the configuration instance (optional) that generates the plot above. */
  Plot::PlotConfig *_config;
  /** Holds the label of the item (title of plot). */
  QString _itemLabel;
  /** Holds the context menu. */
  QMenu *_context_menu;
  /** This action removes the plot. */
  QAction *_removeAct;
};


#endif // __INA_APP_DOCTREE_PLOTWRAPPER_HH__
