#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../timeseries.hh"
#include "retask.hh"

class RETimeSeriesPlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit RETimeSeriesPlot(QList<QString> &selected_species, RETask *data, QObject *parent=0);
};


#endif // LINESGRAPHADAPTOR_HH
