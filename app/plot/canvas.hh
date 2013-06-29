#ifndef __PLOT_CANVAS_HH__
#define __PLOT_CANVAS_HH__

#include "configuration.hh"
#include "figure.hh"


namespace Plot {

/** A base canvas to show plots.
 * @ingroup plot */
class Canvas : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a new Canvas for a plot. */
  explicit Canvas(QWidget *parent=0);

  /** Returns the @c Figure instance or 0 if no figure was set. */
  Figure *getPlot();
  /** Sets the plot figure to be shown. */
  void setPlot(Figure *_plot);
  /** Saves the figure in the given file. */
  void saveAs(const QString &filename, Figure::FileType type);
  /** (Re-) Sets the plot-scheme (@c ConfigScheme). */
  void setScheme(Configuration::Scheme scheme);

protected:
  /** Updates the mappings of the plot being shown. */
  virtual void resizeEvent(QResizeEvent *event);
  /** Handles the paint-event. */
  virtual void paintEvent(QPaintEvent *event);
  /** Handles mouse events. */
  virtual void mousePressEvent(QMouseEvent *event);

protected:
  /** Holds the plot-figure to be shown. */
  Figure *_plot;
};


}

#endif // CANVAS_HH
