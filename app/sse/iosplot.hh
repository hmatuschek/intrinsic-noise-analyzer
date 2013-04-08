#ifndef __IOS_TIMESERIES_PLOT_HH__
#define __IOS_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../models/timeseries.hh"
#include "../views/varianceplot.hh"
#include "iostask.hh"


class IOSEMRETimeSeriesPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit IOSEMRETimeSeriesPlot(const QStringList &selected_species, IOSTask *task, QObject *parent=0);
};


class IOSEMREComparePlot : public LinePlot
{
  Q_OBJECT

public:
  explicit IOSEMREComparePlot(const QStringList &selected_species, IOSTask *task,
                              QObject *parent=0);
};


class IOSEMRECorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSEMRECorrelationPlot(const QStringList &selected_species, IOSTask *task, QObject *parent=0);
};

#endif // LINESGRAPHADAPTOR_HH
