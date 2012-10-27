#ifndef __INA_APP_PARAMSCAN_TASKWRAPPER_HH__
#define __INA_APP_PARAMSCAN_TASKWRAPPER_HH__

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "paramscantask.hh"



class ParamScanTaskWrapper : public TaskItem
{
  Q_OBJECT

public:
  ParamScanTaskWrapper(ParamScanTask *task, QObject *parent=0);

  ParamScanTask *getParamScanTask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
