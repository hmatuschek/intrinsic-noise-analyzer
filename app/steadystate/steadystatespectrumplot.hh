#ifndef __INA_APP_STEADYSTATE_STEADYSTATESPECTRUMPLOT_HH__
#define __INA_APP_STEADYSTATE_STEADYSTATESPECTRUMPLOT_HH__

#include "../plot/plot.hh"
#include "../timeseries.hh"
#include "steadystatetask.hh"


class SteadyStateSpectrumPlot : public Plot::Figure
{
  Q_OBJECT

public:
  SteadyStateSpectrumPlot(Table &spectrum, const iNA::Ast::Unit &species_unit, const iNA::Ast::Unit &time_unit, QObject *parent=0);
};


class SteadyStatePlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit SteadyStatePlot(SteadyStateTask *task, const QStringList selected_species, QObject *parent=0);
};

#endif // __INA_APP_STEADYSTATE_STEADYSTATESPECTRUMPLOT_HH__
