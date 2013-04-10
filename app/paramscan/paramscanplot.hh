#ifndef __INA_APP_PARAMSCAN_PLOT_HH__
#define __INA_APP_PARAMSCAN_PLOT_HH__

#include <QStringList>

// Forward declaration
namespace Plot { class PlotConfig; }
class ParamScanTask;


Plot::PlotConfig *
createParameterScanREPlotConfig(const QStringList &selected_species, ParamScanTask *task);

Plot::PlotConfig *
createParameterScanLNAPlotConfig(const QStringList &selected_species, ParamScanTask *task);

Plot::PlotConfig *
createParameterScanIOSPlotConfig(const QStringList &selected_species, ParamScanTask *task);

Plot::PlotConfig *
createParameterScanLNACVPlotConfig(const QStringList &selected_species, ParamScanTask *task);

Plot::PlotConfig *
createParameterScanLNAFanoPlotConfig(const QStringList &selected_species, ParamScanTask *task);

Plot::PlotConfig *
createParameterScanIOSCVPlotConfig(const QStringList &selected_species, ParamScanTask *task);

Plot::PlotConfig *
createParameterScanIOSFanoPlotConfig(const QStringList &selected_species, ParamScanTask *task);


#endif
