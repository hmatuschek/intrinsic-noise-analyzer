#ifndef __IOS_TIMESERIES_PLOT_HH__
#define __IOS_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../timeseries.hh"


class IOSLNATimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSLNATimeSeriesPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};


class IOSEMRETimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSEMRETimeSeriesPlot(size_t num_species, Table *data,
                                 const QString &species_unit, const QString &time_unit,
                                 QObject *parent=0);
};


class IOSEMREComparePlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSEMREComparePlot(size_t num_species, Table *data,
                              const QString &species_unit, const QString &time_unit,
                              QObject *parent=0);
};


class IOSLNACorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSLNACorrelationPlot(size_t num_species, Table *data,
                                 const QString &time_unit,
                                 QObject *parent=0);
};


class IOSEMRECorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSEMRECorrelationPlot(size_t num_species, Table *data,
                                  const QString &time_unit,
                                  QObject *parent=0);
};

#endif // LINESGRAPHADAPTOR_HH
