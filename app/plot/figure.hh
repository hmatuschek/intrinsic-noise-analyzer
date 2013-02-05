#ifndef __PLOT_FIGURE_HH__
#define __PLOT_FIGURE_HH__


#include <QPen>
#include <QString>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <eigen3/Eigen/Eigen>


#include "axis.hh"
#include "axisticks.hh"
#include "legend.hh"
#include "configuration.hh"
#include "../tinytex/tinytex.hh"


namespace Plot {

class Graph;
class GraphStyle;
class LineGraph;
class VarianceLineGraph;

/**
 * Implements the plotting figure, a QGraohicsScene holding all the graphics elements
 * assembling the plot.
 *
 * @ingroup plot
 */
class Figure : public QGraphicsScene
{
  Q_OBJECT

public:
  /** Specifies the possible legend-positions. */
  typedef enum {
    TOP_LEFT,     ///< Legend in top-left corner.
    TOP_RIGHT,    ///< Legend in top-right corner.
    BOTTOM_LEFT,  ///< Legend in bottom-left corner.
    BOTTOM_RIGHT  ///< Legend in bottom-right corner.
  } LegendPos;

  /** Specifies the possible file-types to save a plot. */
  typedef enum {
    FILETYPE_PNG,  ///< Save plot as PNG image (bitmap)
    FILETYPE_SVG   ///< Save plot as SVG image (vector)
  } FileType;


public:
  /** Constructs a new plot-figure.
   * @param title Specifies the optional plot-title.
   * @param parent Specifies the parent object. */
  explicit Figure(const QString &_title="", QObject *parent = 0);

  /** Resets the title of the figure. */
  void setTitle(const QString &_title);
  /** Returns the title of the figure. */
  QString getTitle() const;

  /** \deprecated This should be moved to @c Plot::ConfigScheme */
  GraphStyle getStyle(int n) const;

  /** Adds a graph to the plot-legend with the given label. */
  void addToLegend(const QString &label, Graph *graph);

  /** (Re-) Sets the label of the x-axis. */
  void setXLabel(const QString &label);
  /** (Re-) Sets the label of the y-axis. */
  void setYLabel(const QString &label);

  /** Returns the axis object of the figure. */
  Axis *getAxis();

  /** Saves the figure as an image in the given file. */
  void save(const QString &filename, FileType type);

  /** Enables/Disables the measure feature. */
  void enableMesure(bool enabled=true);
  /** Shows a measure at the given point if enabled by @c enableMeasure. */
  void showMeasure(const QPointF &point);


public slots:
  /** Replot. */
  void updateAxes();
  /** (Re-) Sets the plot-scheme. */
  void setScheme(Configuration::Scheme scheme);


protected:
  /** Holds the current math context to render the title. */
  MathContext _math_ctx;
  /** Holds the current extend of the rendered title. */
  QSizeF _titlesize;
  /** Holds the plot title text. */
  QString _titletext;

  /** Holds the text graphics item of the plot title. */
  QGraphicsItem *_title;
  /** Holds a reference to the Axis object of the plot. */
  Axis *_axis;
  /** Holds the position of the legend. */
  LegendPos _legend_pos;
  /** Holds the legend instance of the plot. */
  Legend *_legend;
  /** List of graph styles for automatic styles of graphs. */
  QList<GraphStyle> _styles;
  /** If measures are enabled. */
  bool _measures_enabled;
};


}

#endif // PLOT_HH
