#ifndef __PLOT_LINEGRAPH_HH__
#define __PLOT_LINEGRAPH_HH__

#include "graph.hh"
#include "extent.hh"

#include <QPainterPath>
#include <QGraphicsItem>


namespace Plot {

class Axis;

/**
 * Implements a simple line graph.
 *
 * @ingroup plot
 */
class LineGraph : public Graph
{
  Q_OBJECT

protected:
  /**
   * Holds the path for the graph.
   */
  QPainterPath      path;

  /**
   * Holds the graph as a graphics element.
   */
  QGraphicsPathItem *path_item;

  /**
   * Holds the extent (min. and max. values in x- and y-direction) of the graph.
   */
  Extent extent;


public:
  /**
   * Constructs a simple line-graph with the given style.
   */
  explicit LineGraph(const GraphStyle &style);

  /**
   * Retunrs the extent needed by this graph.
   */
  virtual Extent getExtent();

  /**
   * Appends a point to the graph.
   */
  void addPoint(double x, double y);

  /**
   * Appends a point to the graph.
   */
  void addPoint(Eigen::VectorXd &point);

  /**
   * Updates the plot and plot-range.
   */
  void commit();

  /**
   * Redraws the graph.
   */
  virtual void updateGraph(Mapping &mapping);
};


}


#endif // LINEGRAPH_HH
