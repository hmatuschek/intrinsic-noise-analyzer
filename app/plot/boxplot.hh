#ifndef __FLUC_PLOT_BOXPLOT_HH__
#define __FLUC_PLOT_BOXPLOT_HH__

#include "graph.hh"

namespace Plot {


/**
 * Implements a simple box-plot as a graph.
 *
 * @ingroup plot
 */
class BoxPlot : public Graph
{
  Q_OBJECT

protected:
  /**
   * Holds the values of the boxes (heights).
   */
  QList<double> values;

  /**
   * Holds the (optional) std. deviation custom the box.
   */
  QList<double> stddevs;

  /**
   * Holds the graphics elements representing the boxes.
   */
  QList<QGraphicsPathItem *> boxes;

  /**
   * Holds the graphics elements representing the error-bars.
   */
  QList<QGraphicsPathItem *> error_bars;

  /**
   * Holds the box-number.
   */
  size_t box_num;

  /**
   * Total number of boxes.
   */
  size_t num_boxes;

  /**
   * Margin between boxes.
   */
  double margin;


public:
  /**
   * Constructs a new box-plot.
   */
  explicit BoxPlot(const GraphStyle &style, size_t box_num, size_t num_boxes, QObject *parent=0);

  /**
   * Adds a box to the graph.
   */
  void addBox(double value, double stddev=0.0);

  /**
   * Returns the extent.
   */
  virtual Plot::Extent getExtent();

  /**
   * Updates the box-plot.
   */
  virtual void updateGraph(Plot::Mapping &mapping);
};


}

#endif // BOXPLOT_HH
