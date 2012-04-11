#ifndef STEADYSTATESPECTRUMPLOT_HH
#define STEADYSTATESPECTRUMPLOT_HH

#include "../plot/plot.hh"
#include "../timeseries.hh"
#include "lnasteadystatetask.hh"


class SteadyStateSpectrumPlot : public Plot::Figure
{
  Q_OBJECT

public:
  SteadyStateSpectrumPlot(Table &spectrum, const Fluc::Ast::Unit &species_unit, const Fluc::Ast::Unit &time_unit, QObject *parent=0);
};


class SteadyStatePlot : public Plot::Figure
{
  Q_OBJECT

public:
  explicit SteadyStatePlot(LNASteadyStateTask *task, QObject *parent=0);
};

#endif // STEADYSTATESPECTRUMPLOT_HH
