#ifndef __IOS_TIMESERIES_PLOT_HH__
#define __IOS_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../timeseries.hh"


class IOSTimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit IOSTimeSeriesPlot(size_t num_species, Table *data,
                             const QString &species_unit, const QString &time_unit,
                             QObject *parent=0);
};


#endif // LINESGRAPHADAPTOR_HH
