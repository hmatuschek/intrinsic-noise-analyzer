#include "ssaparamscantaskwrapper.hh"
#include "../models/application.hh"
#include "ssaparamscantaskview.hh"


SSAParamScanTaskWrapper::SSAParamScanTaskWrapper(SSAParamScanTask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


SSAParamScanTask *
SSAParamScanTaskWrapper::getParamScanTask()
{
  return static_cast<SSAParamScanTask *>(this->task);
}


bool
SSAParamScanTaskWrapper::providesView() const
{
  return true;
}


QWidget *
SSAParamScanTaskWrapper::createView()
{
  return new SSAParamScanTaskView(this);
}
