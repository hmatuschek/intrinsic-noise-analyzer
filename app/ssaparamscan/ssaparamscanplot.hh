#ifndef __INA_APP_SSAPARAMSCAN_PLOT_HH__
#define __INA_APP_SSAPARAMSCAN_PLOT_HH__

#include <QStringList>

// Forward declaration
namespace Plot { class PlotConfig; class Figure; }
class SSAParamScanTask;


Plot::PlotConfig *
createSSAParameterScanPlotConfig(const QStringList &selected_species, SSAParamScanTask *task);

Plot::Figure *
createSSAParameterScanPlot(const QStringList &selected_species, SSAParamScanTask *task);

Plot::PlotConfig *
createSSAParameterScanCVPlotConfig(const QStringList &selected_species, SSAParamScanTask *task);

Plot::Figure *
createSSAParameterScanCVPlot(const QStringList &selected_species, SSAParamScanTask *task);

Plot::PlotConfig *
createSSAParameterScanFanoPlotConfig(const QStringList &selected_species, SSAParamScanTask *task);

Plot::Figure *
createSSAParameterScanFanoPlot(const QStringList &selected_species, SSAParamScanTask *task);

#endif
