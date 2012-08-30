#include "ssscantaskwrapper.hh"
#include "../application.hh"
#include "ssscantaskview.hh"


SSScanTaskWrapper::SSScanTaskWrapper(SSScanTask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


SSScanTask *
SSScanTaskWrapper::getSSScanTask()
{
  return static_cast<SSScanTask *>(this->task);
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
