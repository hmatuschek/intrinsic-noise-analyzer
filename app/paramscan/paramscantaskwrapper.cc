#include "paramscantaskwrapper.hh"
#include "../models/application.hh"
#include "paramscantaskview.hh"


ParamScanTaskWrapper::ParamScanTaskWrapper(ParamScanTask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


ParamScanTask *
ParamScanTaskWrapper::getParamScanTask()
{
  return static_cast<ParamScanTask *>(this->task);
}


bool
ParamScanTaskWrapper::providesView() const
{
  return true;
}


QWidget *
ParamScanTaskWrapper::createView()
{
  return new ParamScanTaskView(this);
}
