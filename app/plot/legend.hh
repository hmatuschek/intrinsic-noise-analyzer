#ifndef __PLOT_LEGEND_HH__
#define __PLOT_LEGEND_HH__

#include <QList>
#include <QPair>
#include <QGraphicsItemGroup>

#include "graph.hh"
#include "configuration.hh"


namespace Plot {


/** Represents an item (graph) in a legend.
 * @ingroup plot */
class LegendItem : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

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
};



/** Represents a plot-legend, a collection of @c LegendItem instances.
 * @ingroup plot */
class Legend : public QObject, public QGraphicsItemGroup
{
  Q_OBJECT

public:
  /** Constructs an empty legend. */
  Legend(QObject *parent=0);

  /** Adds a graph to the legend with the given label. */
  void addGraph(const QString &label, Graph *graph);
  /** Returns the bounding-box of the legend. */
  virtual QRectF boundingRect() const;

public slots:
  /** Updates the layout of the legend. */
  void updateLayout();
  /** Updates the scheme of all legend-items: */
  void setScheme(Configuration::Scheme scheme);

protected slots:
  /** Updates the Layout of the legend. */
  void updateBB();

protected:
  /** Holds the list of legend-items. */
  QList<LegendItem *> _items;
  /** The background of the legend. */
  QGraphicsPathItem *_background;
  /** Left margin. */
  double _margin_left;
  /** Bottom margin. */
  double _margin_bottom;
  /** Right margin. */
  double _margin_right;
  /** Top margin. */
  double _margin_top;
  /** Space between legend-items. */
  double _line_spacing;
  /** Holds the bounding-box of the legend. */
  QRectF _bb;
};


}

#endif // LEGEND_HH
