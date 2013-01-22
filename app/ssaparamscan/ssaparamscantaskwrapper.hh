#ifndef __INA_APP_SSAPARAMSCAN_TASKWRAPPER_HH__
#define __INA_APP_SSAPARAMSCAN_TASKWRAPPER_HH__

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "ssaparamscantask.hh"



class SSAParamScanTaskWrapper : public TaskItem
{
  Q_OBJECT

public:
  SSAParamScanTaskWrapper(SSAParamScanTask *task, QObject *parent=0);

  SSAParamScanTask *getParamScanTask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
