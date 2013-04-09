#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__
#include <QStringList>

// Forward declarations:
namespace Plot { class PlotConfig; }
class RETask;

/** Assembles the configuration of a RE Timeseries plot and creates the figure from it. */
Plot::PlotConfig *createRETimeSeriesPlotConfig(QStringList &selected_species, RETask *task);

#endif // LINESGRAPHADAPTOR_HH
