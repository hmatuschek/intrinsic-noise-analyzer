#ifndef __INA_APP_VIEWS_VARIANCEPLOT_HH__
#define __INA_APP_VIEWS_VARIANCEPLOT_HH__

#include "../plot/figure.hh"
#include "../plot/variancelinegraph.hh"


/**
 * General purpose variance plot.
 *
 * Only 100 points of the timeseries are plotted.
 */
class VariancePlot : public Plot::Figure
{
  Q_OBJECT

public:
  /** Default constructor, with title. */
  VariancePlot(const QString &title, QObject *parent=0);
  /** Adds a graph to the plot. */
  void addVarianceGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean,
                        const Eigen::VectorXd &var, const QString &label="");
};

#endif // __INA_APP_VIEWS_VARIANCEPLOT_HH__
