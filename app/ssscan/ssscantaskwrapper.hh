#ifndef __INA_APP_SSSCAN_TASKWRAPPER_HH__
#define __INA_APP_SSSCAN_TASKWRAPPER_HH__

#include "../doctree/taskitem.hh"
#include "../doctree/plotitem.hh"
#include "ssscantask.hh"



class SSScanTaskWrapper : public TaskItem
{
  Q_OBJECT

public:
  SSScanTaskWrapper(SSScanTask *task, QObject *parent=0);

  SSScanTask *getSSScanTask();

  virtual bool providesView() const;
  virtual QWidget *createView();
};


#endif // LNATASKWRAPPER_HH
