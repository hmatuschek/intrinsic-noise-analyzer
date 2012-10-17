#ifndef __INA_APP_STEADYSTATE_STEADYSTATETASKWRAPPER_HH__
#define __INA_APP_STEADYSTATE_STEADYSTATETASKWRAPPER_HH__

#include "steadystatetask.hh"
#include "../doctree/taskitem.hh"
#include "../models/tablewrapper.hh"



class SteadyStateTaskWrapper : public TaskItem
{
  Q_OBJECT

protected:
  TableWrapper spectrum_wrapper;

public:
  explicit SteadyStateTaskWrapper(SteadyStateTask *task, QObject *parent=0);

  SteadyStateTask *getSteadyStateTask();

  virtual bool providesView() const;
  virtual QWidget *createView();

  TableWrapper *getSpectrum();
};


#endif // __INA_APP_STEADYSTATE_STEADYSTATETASKWRAPPER_HH__
