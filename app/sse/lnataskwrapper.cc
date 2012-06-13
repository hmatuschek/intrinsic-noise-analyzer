#include "lnataskwrapper.hh"
#include "../application.hh"
#include "lnataskview.hh"


LNATaskWrapper::LNATaskWrapper(LNATask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


LNATask *
LNATaskWrapper::getLNATask()
{
  return static_cast<LNATask *>(this->task);
}


bool
LNATaskWrapper::providesView() const
{
  return true;
}


QWidget *
LNATaskWrapper::createView()
{
  return new LNATaskView(this);
}
