#ifndef LNASTEADYSTATETASKWRAPPER_HH
#define LNASTEADYSTATETASKWRAPPER_HH

#include "lnasteadystatetask.hh"
#include "../doctree/taskitem.hh"
#include "../models/tablewrapper.hh"



class LNASteadyStateTaskWrapper : public TaskItem
{
  Q_OBJECT

protected:
  TableWrapper spectrum_wrapper;

public:
  explicit LNASteadyStateTaskWrapper(LNASteadyStateTask *task, QObject *parent=0);

  LNASteadyStateTask *getSteadyStateTask();

  virtual bool providesView() const;
  virtual QWidget *createView();

  TableWrapper *getSpectrum();
};


#endif // LNASTEADYSTATETASKWRAPPER_HH
