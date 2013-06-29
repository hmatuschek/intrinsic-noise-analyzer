#ifndef __INA_APP_VIEW_PLOTVIEW_HH__
#define __INA_APP_VIEW_PLOTVIEW_HH__

#include <QWidget>
#include <QComboBox>

class PlotItem;
namespace Plot {
class Canvas;
}


/** Common view of a plot.
 * @ingroup gui */
class PlotView : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a view for the given plot-item. */
  explicit PlotView(PlotItem *plot_item, QWidget *parent = 0);

private slots:
  /** Shows a save-as dialog. */
  void onSavePlot();
  /** Resets the plot-range. */
  void onSetPlotRange();
  /** Will be called if the plot-item gets destroyed. */
  void onPlotDestroy();
  /** Resets the plot-scheme. */
  void onSchemeSelected(int idx);
  /** Re-configures the plot. */
  void onConfigPlot();

private:
  /** Holds a weak reference to the plot item being shown. */
  PlotItem *_plotitem;
  /** Holds the plot-canvas, showing the plot. */
  Plot::Canvas *_canvas;
  /** Combobox of plot-schemes. */
  QComboBox *_schemeBox;
};

#endif // __INA_APP_VIEW_PLOTVIEW_HH__
