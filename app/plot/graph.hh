#ifndef __PLOT_GRAPH_HH__
#define __PLOT_GRAPH_HH__

#include <eigen3/Eigen/Eigen>

#include <QPen>
#include <QObject>
#include <QPainterPath>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>

#include "axis.hh"
#include "extent.hh"


namespace Plot {



/**
 * This class defines the style (width, line style and color) of a graph.
 *
 * @ingroup plot
 */
class GraphStyle
{
protected:
  /**
   * The pen used to draw the graph.
   */
  QPen pen;

  /**
   * Separate fill-color.
   */
  QColor fillColor;


public:
  /**
   * Construct the default (black) graph-style.
   */
  GraphStyle();

  /**
   * Full contructor.
   */
  GraphStyle(Qt::PenStyle style, const QColor &color, double width=2);

  /**
   * Copy constructor.
   */
  GraphStyle(const GraphStyle &other);

  /**
   * Returns the pen for the graph.
   */
  const QPen &getPen() const;

  /**
   * Returns the fill-color for the graph.
   */
  const QColor &getFillColor() const;

  /**
   * Retruns the line-color of the graph, this is the color of the pen.
   */
  QColor getLineColor() const;

  /**
   * Resets the pen-style of the graph.
   */
  void setPenStyle(Qt::PenStyle style);

  /**
   * Resets the fill-color of the graph.
   */
  void setFillColor(const QColor &color);

  /**
   * Resets the line-color of the graph.
   */
  void setLineColor(const QColor &color);

  /**
   * Resets the line-width of the graph.
   */
  void setLineWidth(double w);
};



/**
 * Abstract base class for all graphs.
 *
 * @ingroup plot
 */
class Graph : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

protected:
  /**
   * Holds the style of the graph.
   */
  GraphStyle style;


public:
  /**
   * Base-constructor, with graph-style.
   */
  Graph(const GraphStyle &style, QObject *parent=0);

  /**
   * Returns the extent (min. and max. values in x- and y-direction) of the graph.
   *
   * This method needs to be implemented by any sub-class.
   */
  virtual Extent getExtent() = 0;

  /**
   * Will be called by the @c Axis class to signal a replot() event.
   */
  virtual void updateGraph(Mapping &mapping) = 0;

  /**
   * Returns the graph-style.
   */
  virtual const GraphStyle &getStyle() const;


public slots:
  /**
   * Resets the plot-scheme.
   */
  void setScheme(Configuration::Scheme scheme);


signals:
  /**
   * Will be emmitted, if the graph has been changed (ie. extended).
   */
  void graphModified();
};


}


#endif // __PLOT_GRAPH_HH__
