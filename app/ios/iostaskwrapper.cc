#include "iostaskwrapper.hh"
#include "../application.hh"
#include "iostaskview.hh"


IOSTaskWrapper::IOSTaskWrapper(IOSTask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


IOSTask *
IOSTaskWrapper::getIOSTask()
{
  return static_cast<IOSTask *>(this->task);
}


bool
IOSTaskWrapper::providesView() const
{
  return true;
}


QWidget *
IOSTaskWrapper::createView()
{
  return new IOSTaskView(this);
}
