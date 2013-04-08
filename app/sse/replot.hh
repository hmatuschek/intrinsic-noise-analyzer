#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__

#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../models/timeseries.hh"
#include "../views/varianceplot.hh"
#include "retask.hh"

class RETimeSeriesPlot : public LinePlot
{
  Q_OBJECT

public:
  explicit RETimeSeriesPlot(QList<QString> &selected_species, RETask *task, QObject *parent=0);
};

/** Assembles the configuration of a RE Timeseries plot and creates the figure from it. */
Plot::ConfiguredPlot *createRETimeSeriesPlot(QList<QString> &selected_species, RETask *task);

/** Assembles the configuration of a RE Timeseries plot and creates the figure from it. */
Plot::PlotConfig *createRETimeSeriesPlotConfig(QList<QString> &selected_species, RETask *task);

#endif // LINESGRAPHADAPTOR_HH
