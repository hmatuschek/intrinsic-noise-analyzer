#ifndef SSAPLOT_HH
#define SSAPLOT_HH
#include <QStringList>

namespace Plot { class PlotConfig; }
class SSATask;

Plot::PlotConfig *
createSSAPlotConfig(const QStringList &selected_species, SSATask *task);

Plot::PlotConfig *
createSSACorrelationPlotConfig(const QStringList &selected_species, SSATask *task);

#endif // SSAPLOT_HH
