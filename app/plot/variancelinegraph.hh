#ifndef __PLOT_VARIANCELINEGRAPH_HH__
#define __PLOT_VARIANCELINEGRAPH_HH__

#include "graph.hh"

namespace Plot {

/**
 * A simple line graph with variance areas.
 *
 * @ingroup plot
 */
class VarianceLineGraph : public Graph
{
  Q_OBJECT

protected:
  /**
   * The path of the mean value.
   */
  QPainterPath      path;

  /**
   * The path of the upper limit (mean + std. dev.).
   */
  QPainterPath      upper_var_path;

  /**
   * The path of the lower limit (mean - std. dev.).
   */
  QPainterPath      lower_var_path;

  /**
   * The mean-value path as a graphics element.
   */
  QGraphicsPathItem *path_item;

  /**
   * The variance path as a graphics element.
   */
  QGraphicsPathItem *var_path_item;

  /**
   * Min & Max values.
   */
  Extent extent;


public:
  /**
   * Constructs a new variance graph with given graph style.
   */
  explicit VarianceLineGraph(const GraphStyle &style);

  /**
   * Returns the extend of the graph.
   */
  virtual Extent getExtent();

  /**
   * Updates the graphics elements using the given mapping.
   */
  virtual void updateGraph(Mapping &mapping);

  /**
   * Appends a point to the graph.
   */
  void addPoint(double x, double y, double std_dev);

  /**
   * Appends a point to the graph.
   */
  void addPoint(Eigen::VectorXd &point);

  /**
   * Applies any changes, aka. replot().
   */
  void commit();
};


}

#endif // VARIANCELINEGRAPH_HH
