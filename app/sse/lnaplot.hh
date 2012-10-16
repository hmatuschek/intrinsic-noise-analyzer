#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"
#include "../timeseries.hh"
#include "lnatask.hh"
#include "../views/varianceplot.hh"


/** Implements a simple quick plot for the RE mean and LAN var. */
class LNATimeSeriesPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit LNATimeSeriesPlot(QList<QString> &selected_species, LNATask *task, QObject *parent=0);
};


/** Implements a simple correlation plot for LNA covariances. */
class LNACorrelationPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit LNACorrelationPlot(QList<QString> &selected_species, LNATask *task, QObject *parent=0);
};


#endif // LINESGRAPHADAPTOR_HH
