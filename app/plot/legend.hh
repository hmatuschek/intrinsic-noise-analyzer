#ifndef __PLOT_LEGEND_HH__
#define __PLOT_LEGEND_HH__

#include <QList>
#include <QPair>
#include <QGraphicsItemGroup>

#include "graph.hh"
#include "configuration.hh"


namespace Plot {


/**
 * Represents an item (graph) in a legend.
 *
 * @ingroup plot
 */
class LegendItem : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

protected:
  /** Holds a weak reference to the graph item. */
  Graph *_graph;
  /** Holds the "sample" of the graph (a short line with the same style as the graph). */
  QGraphicsLineItem *_line;
  /** Holds the label text. */
  QString _label_text;
  /** Holds the label for the graph in the legend. */
  QGraphicsItem *_label;
  /** Holds the size of the rendered label item. */
  QSizeF _label_size;
  /** Space between sample and label. */
  double _space;
  /** Length of the sample. */
  double _sample_length;
  /** The bounding-box of the legend-item. */
  QRectF _bb;


public:
  /** Constructs a legend-item for the given graph and with the given label. */
  explicit LegendItem(const QString &_label, Graph *_graph, QObject *parent=0);

  /** Returns the bounding-box of the item. */
  virtual QRectF boundingRect() const;

  /** Updates the legend-item. */
  void updateLayout();


public slots:
  /** (Re-) Sets the plot-scheme. */
  void setScheme(Configuration::Scheme scheme);
};



/**
 * Represents a plot-legend, a collection of @c LegendItem instances.
 *
 * @ingroup plot
 */
class Legend : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

protected:
  /**
   * Holds the list of legend-items.
   */
  QList<LegendItem *> items;

  /**
   * The background of the legend.
   */
  QGraphicsPathItem *background;

  /**
   * Left margin.
   */
  double margin_left;

  /**
   * Bottom margin.
   */
  double margin_bottom;

  /**
   * Right margin.
   */
  double margin_right;

  /**
   * Top margin.
   */
  double margin_top;

  /**
   * Space between legend-items.
   */
  double line_spacing;

  /**
   * Holds the bounding-box of the legend.
   */
  QRectF bb;


public:
  /**
   * Constructs an empty legend.
   */
  Legend(QObject *parent=0);

  /**
   * Adds a graph to the legend with the given label.
   */
  void addGraph(const QString &label, Graph *graph);

  /**
   * Returns the bounding-box of the legend.
   */
  virtual QRectF boundingRect() const;


public slots:
  /**
   * Updates the scheme of all legend-items:
   */
  void setScheme(Configuration::Scheme scheme);


protected:
  /**
   * Updates the BB of the legend.
   */
  void updateBB();
};


}

#endif // LEGEND_HH
