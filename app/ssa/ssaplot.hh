#ifndef SSAPLOT_HH
#define SSAPLOT_HH

#include "../views/varianceplot.hh"
#include "ssatask.hh"


/** Simply plots the mean and std. dev custom the selected species. */
class SSAPlot : public LinePlot
{
  Q_OBJECT

public:
  SSAPlot(const QStringList &selected_species, SSATask *task, QObject *parent=0);
};


/** Plots the correlation coefficients. */
class SSACorrelationPlot : public LinePlot
{
  Q_OBJECT

public:
  SSACorrelationPlot(const QStringList &selected_species, SSATask *task, QObject *parent=0);
};


#endif // SSAPLOT_HH
