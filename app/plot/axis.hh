#ifndef __PLOT_AXIS_HH__
#define __PLOT_AXIS_HH__

#include <QObject>
#include <QGraphicsItemGroup>
#include <eigen3/Eigen/Eigen>

#include "extent.hh"
#include "axisticks.hh"
#include "mapping.hh"
#include "configuration.hh"


namespace Plot {

class Axis;
class Graph;
class GraphStyle;
class LineGraph;


/** A simple graphics element to display a cross with some values (x,y). */
class MeasureItem : public QObject, public QGraphicsItemGroup
{
public:
  /** Constructor. */
  MeasureItem(Axis *parent);

  /** Reset values to display. */
  void setValues(const QPointF &values);
  /** Returns the current values. */
  const QPointF &getValues() const;

private:
  /** Holds the current values. */
  QPointF _values;
  /** Holds the label item. */
  QGraphicsTextItem *_label;
};



/** Displays the collection of graphs in a figure, also implements clipping.
 * @ingroup plot */
class GraphGroup : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

public:
  /** Constructs a new (empty) graph group. */
  explicit GraphGroup(QObject *parent=0);

  /** (Re-) Sets the clipping. */
  void setClipping(const QRectF &rect);
  /** Returns the bounding-box == clipping area. */
  virtual QRectF boundingRect() const;

protected:
  /** Holds the clipping-area. */
  QRectF _clipping;
};



/**
 * This class defines the coordinate system for plots.
 *
 * @ingroup plot
 */
class Axis : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

public:
  /** Constructs an empty Axis instance. */
  explicit Axis(QObject *parent=0);

  /** Adds a graph to the axis. */
  void addGraph(Graph *graph);
  /** Returns the number of graphs in held by the axis. */
  size_t getNumGraphs();

  /** Sets the size of the axis. To update the graphs, call @c updatePlotSize and @c forceRedraw. */
  void setAxisSize(const QSizeF &size);

  /** (Re-) Sets the x-range. */
  void setXRange(const Range &range);
  /** (Re-) Sets the x-range. */
  void setXRange(double min, double max);
  /** Returns the x-range. */
  Range getXRange();
  /** Resets the x-range-policy. */
  void setXRangePolicy(const RangePolicy &policy);
  /** Returns the x-range-policy. */
  RangePolicy getXRangePolicy();
  /** (Re-) Sets the axis-ticks for the x-axis. */
  void setXTicks(AxisTicks *ticks);
  /** Sets the label of the x axis. */
  void setXLabel(const QString &label);

  /** (Re-) Sets the y-range. */
  void setYRange(const Range &range);
  /** (Re-) Sets the y-range. */
  void setYRange(double min, double max);
  /** Returns the y-range. */
  Range getYRange();
  /** (Re-) Sets the y-range-policy. */
  void setYRangePolicy(const RangePolicy &policy);
  /** Returns the y-range-policy. */
  RangePolicy getYRangePolicy();
  /** (Re-) Sets the y-axis ticks. */
  void setYTicks(AxisTicks *ticks);
  /** Sets the label of the y axis. */
  void setYLabel(const QString &label);

  /** Shows a vertical line at x=0. */
  void showXZero(bool show);
  /** Shows a horizontal line at y=0. */
  void showYZero(bool show);

  /** (Re-) Sets the mapping for the x-axis. */
  void setXMapFunction(MapFunction *func);
  /** (Re-) Sets the mapping for the y-axis. */
  void setYMapFunction(MapFunction *func);
  /** Returns the mapping for the x- and y-axis. */
  Mapping *getMapping();
  /** Returns the coordinates of the plotting area. */
  QRectF getPlotArea() const;

  /** Shows a measure at the given coordinates. */
  void showMeasure(const QPointF &point);


public slots:
  /** Updates the @c Mapping and @c GraphGroup (clipping) to match new plot area.
   * Also resizes and updates the position of the @c GraphGroup and @c AxisTicks and labels and
   * forces a redraw of all graphs. */
  void updatePlotSize();
  /** Updates the @c Mapping, @c GraphGroup (clipping) and @c AxisTicks to match new plot range.
   * Also forces a redraw of the graphs. */
  void updatePlotRange();
  /** Internal used function to update graphs etc. */
  void forceRedraw();
  /** Applies the given Scheme on the axis. */
  void setScheme(Configuration::Scheme scheme);


protected:
  /** Holds the mapping function for the axis. */
  Mapping *mapping;
  /** Available size for the axis including the axis ticks. */
  QSizeF axis_size;
  /** Holds the top or bottom ticks for the x-axis. */
  AxisTicks *xticks;
  /** Holds the left or right ticks for the y-axis. */
  AxisTicks *yticks;
  /** Holds all graphs as grahics items. */
  GraphGroup *graph_group;
  /** Holds the list of all graphs. */
  QList<Graph *> graphs;
  /** Holds the x==0 line as a graphics element. */
  QGraphicsLineItem *x_zero;
  /** Holds the y==0 line as a graphics element. */
  QGraphicsLineItem *y_zero;
  /** Holds the measure instance. */
  MeasureItem *_measure;
};


}

#endif // __PLOT_AXIS_HH__
