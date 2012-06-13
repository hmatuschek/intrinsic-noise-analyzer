#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"
#include "../timeseries.hh"


/**
 * Collects data from a @c Table and updates the
 * @c VarianceLinesGraph instance.
 */
class LNATimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit LNATimeSeriesPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};



class EMRETimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit EMRETimeSeriesPlot(size_t num_species, Table *data,
                              const QString &species_unit, const QString &time_unit,
                              QObject *parent=0);
};



class LNACorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit LNACorrelationPlot(size_t num_species, Table *data, const QString &time_unit,
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
