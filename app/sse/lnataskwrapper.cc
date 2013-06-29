#include "lnataskwrapper.hh"
#include "../models/application.hh"
#include "lnataskview.hh"


LNATaskItem::LNATaskItem(LNATask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


LNATask *
LNATaskItem::getLNATask()
{
  return static_cast<LNATask *>(this->task);
}


bool
LNATaskItem::providesView() const
{
  return true;
}


QWidget *
LNATaskItem::createView()
{
  return new LNATaskView(this);
}
