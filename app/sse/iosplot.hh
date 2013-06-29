#ifndef __INA_APP_IOS_PLOT_HH__
#define __INA_APP_IOS_PLOT_HH__
#include <QStringList>

// Forward declarations
namespace Plot { class PlotConfig; }
class IOSTask;


Plot::PlotConfig *
createIOSEMRETimeSeriesPlotConfig(const QStringList &selected_species, IOSTask *task);

Plot::PlotConfig *
createIOSEMREComparePlotConfig(const QStringList &selected_species, IOSTask *task);

Plot::PlotConfig *
createIOSEMRECorrelationPlotConfig(const QStringList &selected_species, IOSTask *task);

#endif // __INA_APP_IOS_PLOT_HH__
