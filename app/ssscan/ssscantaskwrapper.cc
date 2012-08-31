#include "ssscantaskwrapper.hh"
#include "../application.hh"
#include "ssscantaskview.hh"


SSScanTaskWrapper::SSScanTaskWrapper(ParamScanTask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


ParamScanTask *
SSScanTaskWrapper::getSSScanTask()
{
  return static_cast<ParamScanTask *>(this->task);
}


bool
SSScanTaskWrapper::providesView() const
{
  return true;
}


QWidget *
SSScanTaskWrapper::createView()
{
  return new SSScanTaskView(this);
}
