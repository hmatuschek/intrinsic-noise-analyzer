#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"
#include "../timeseries.hh"
#include "lnatask.hh"


/** Implements a simple quick plot for the RE mean and LAN var. */
class LNATimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit LNATimeSeriesPlot(QList<QString> &selected_species, LNATask *task, QObject *parent=0);
};


/** Implements a simple correlation plot for LNA covariances. */
class LNACorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit LNACorrelationPlot(QList<QString> &selected_species, LNATask *task, QObject *parent=0);
};


class EMRETimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit EMRETimeSeriesPlot(size_t num_species, Table *data,
                              const QString &species_unit, const QString &time_unit,
                              QObject *parent=0);
};




/**
 * @deprecated Unused.
 */
class LNAVadilityPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit LNAVadilityPlot(size_t num_species, Table *data, QObject *parent=0);
};

#endif // LINESGRAPHADAPTOR_HH
