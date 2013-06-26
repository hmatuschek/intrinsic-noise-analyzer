#include "iostaskwrapper.hh"
#include "iostaskview.hh"
#include "../models/application.hh"


IOSTaskItem::IOSTaskItem(IOSTask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


IOSTask *
IOSTaskItem::getIOSTask()
{
  return static_cast<IOSTask *>(this->task);
}


bool
IOSTaskItem::providesView() const
{
  return true;
}


QWidget *
IOSTaskItem::createView()
{
  return new IOSTaskView(this);
}
