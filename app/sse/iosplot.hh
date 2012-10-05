#ifndef __IOS_TIMESERIES_PLOT_HH__
#define __IOS_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../timeseries.hh"
#include "../views/varianceplot.hh"
#include "iostask.hh"


class IOSEMRETimeSeriesPlot : public VariancePlot
{
  Q_OBJECT

public:
  explicit IOSEMRETimeSeriesPlot(const QStringList &selected_species, IOSTask *task, QObject *parent=0);
};


/**
 * Trivial specialization to the variance plot.
 *
 * This class plots the mean and std.dev. determined by the LNA.
 */
class IOSLNATimeSeriesPlot : public VariancePlot
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit IOSLNATimeSeriesPlot(size_t num_species, Table *data,
                                const QString &species_unit, const QString &time_unit,
                                QObject *parent=0);
};


class IOSEMREComparePlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSEMREComparePlot(const QStringList &selected_species, IOSTask *task,
                              QObject *parent=0);
};


class IOSLNACorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSLNACorrelationPlot(IOSTask *taks,
                                 const QString &time_unit,
                                 QObject *parent=0);
};


class IOSEMRECorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSEMRECorrelationPlot(const QStringList &selected_species, IOSTask *task, QObject *parent=0);
};

#endif // LINESGRAPHADAPTOR_HH
