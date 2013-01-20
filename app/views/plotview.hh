#ifndef __INA_APP_VIEW_PLOTVIEW_HH__
#define __INA_APP_VIEW_PLOTVIEW_HH__

#include <QWidget>
#include <QComboBox>

#include "../plot/canvas.hh"
#include "../plot/plotrangedialog.hh"
#include "../doctree/plotitem.hh"


/** Common view of a plot.
 * @ingroup gui */
class PlotView : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a view for the given plot-item. */
  explicit PlotView(PlotItem *plot_wrapper, QWidget *parent = 0);

private:
  /** Holds the plot-canvas, showing the plot. */
  Plot::Canvas *canvas;

private slots:
  /** Shows a save-as dialog. */
  void onSavePlot();
  /** Resets the plot-range. */
  void onSetPlotRange();
  /** Will be called if the plot-item gets destroyed. */
  void onPlotDestroy();
  /** Resets the plot-scheme. */
  void onSchemeSelected(int idx);

private:
  /** Combobox of plot-schemes. */
  QComboBox *schemeBox;
};

#endif // __INA_APP_VIEW_PLOTVIEW_HH__
