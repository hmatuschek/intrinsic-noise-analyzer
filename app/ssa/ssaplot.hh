#ifndef SSAPLOT_HH
#define SSAPLOT_HH

#include "../plot/plot.hh"
#include "ssatask.hh"


class SSAPlot : public Plot::Figure
{
  Q_OBJECT

public:
  SSAPlot(SSATask *task, QObject *parent=0);
};


class SSACorrelationPlot : public Plot::Figure
{
  Q_OBJECT

public:
  SSACorrelationPlot(SSATask *task, QObject *parent=0);
};


#endif // SSAPLOT_HH
