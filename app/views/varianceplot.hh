#ifndef __INA_APP_VIEWS_VARIANCEPLOT_HH__
#define __INA_APP_VIEWS_VARIANCEPLOT_HH__

#include "../plot/figure.hh"
#include "../plot/variancelinegraph.hh"
#include "../plot/linegraph.hh"


/**
 * General purpose line plot.
 * Only 100 points of the timeseries are plotted.
 */
class LinePlot : public Plot::Figure
{
  Q_OBJECT

public:
  /** Default constructor, with title. */
  LinePlot(const QString &_title, QObject *parent=0);
  /** Adds a graph to the plot. */
  void addLineGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean,
                        const QString &label="");
  /** Adds a graph to the plot. */
  void addVarianceGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean,
                        const Eigen::VectorXd &var, const QString &label="");
};


/**
 * General purpose variance plot.
 *
 * Only 100 points of the timeseries are plotted.
 * @deprecated Use @c LinePlot instead.
 */
class VariancePlot : public Plot::Figure
{
  Q_OBJECT

public:
  /** Default constructor, with title. */
  VariancePlot(const QString &_title, QObject *parent=0);
  /** Adds a graph to the plot. */
  void addVarianceGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean,
                        const Eigen::VectorXd &var, const QString &label="");
};

#endif // __INA_APP_VIEWS_VARIANCEPLOT_HH__
