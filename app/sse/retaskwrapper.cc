#include "retaskwrapper.hh"
#include "../models/application.hh"
#include "retaskview.hh"


RETaskWrapper::RETaskWrapper(RETask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


RETask *
RETaskWrapper::getRETask()
{
  return static_cast<RETask *>(this->task);
}


bool
RETaskWrapper::providesView() const
{
  return true;
}


QWidget *
RETaskWrapper::createView()
{
  return new RETaskView(this);
}
