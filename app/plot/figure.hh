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
  /**
   * Specifies the possible legend-positions.
   */
  typedef enum {
    TOP_LEFT,     ///< Legend in top-left corner.
    TOP_RIGHT,    ///< Legend in top-right corner.
    BOTTOM_LEFT,  ///< Legend in bottom-left corner.
    BOTTOM_RIGHT  ///< Legend in bottom-right corner.
  } LegendPos;

  /**
   * Specifies the possible file-types to save a plot.
   */
  typedef enum {
    FILETYPE_PNG,  ///< Save plot as PNG image (bitmap)
    FILETYPE_SVG   ///< Save plot as SVG image (vector)
  } FileType;


protected:
  /**
   * Holds the text graphics item of the plot title.
   */
  QGraphicsTextItem *title;

  /**
   * Holds a reference to the Axis object of the plot.
   */
  Axis *axis;

  /**
   * Holds the position of the legend.
   */
  LegendPos legend_pos;

  /**
   * Holds the legend instance of the plot.
   */
  Legend *legend;

  /**
   * List of graph styles for automatic styles of graphs.
   */
  QList<GraphStyle> styles;


public:
  /**
   * Constructs a new plot-figure.
   *
   * @param title Specifies the optional plot-title.
   * @param parent Specifies the parent object.
   */
  explicit Figure(const QString &title="", QObject *parent = 0);

  /**
   * Resets the title of the figure.
   */
  void setTitle(const QString &title);

  /**
   * Returns the title of the figure.
   */
  QString getTitle() const;

  /**
   * \deprecated This should be moved to @c Plot::ConfigScheme
   */
  GraphStyle getStyle(int n) const;

  /**
   * Adds a graph to the plot-legend with the given label.
   */
  void addToLegend(const QString &label, Graph *graph);

  /**
   * (Re-) Sets the label of the x-axis.
   */
  void setXLabel(const QString &label);

  /**
   * (Re-) Sets the label of the y-axis.
   */
  void setYLabel(const QString &label);

  /**
   * Returns the axis object of the figure.
   */
  Axis *getAxis();

  /**
   * Saves the figure as an image in the given file.
   */
  void save(const QString &filename, FileType type);


public slots:
  /**
   * Replot.
   */
  void updateAxes();

  /**
   * (Re-) Sets the plot-scheme.
   */
  void setScheme(Configuration::Scheme scheme);


protected:
  /** Handles mouse clicks inside the plot. */
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
};


}

#endif // PLOT_HH
