#ifndef __LNA_TIMESERIES_PLOT_HH__
#define __LNA_TIMESERIES_PLOT_HH__

#include <QStringList>
// Forward declarations
namespace Plot { class PlotConfig; }
class LNATask;


Plot::PlotConfig *
createLNATimeSeriesPlotConfig(QStringList &selected_species, LNATask *task);

Plot::PlotConfig *
createLNACorrelationPlotConfig(QStringList &selected_species, LNATask *task);

#endif // LINESGRAPHADAPTOR_HH
