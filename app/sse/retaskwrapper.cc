#include "retaskwrapper.hh"
#include "../models/application.hh"
#include "retaskview.hh"


RETaskItem::RETaskItem(RETask *task, QObject *parent)
  : TaskItem(task, parent)
{
}


RETask *
RETaskItem::getRETask()
{
  return static_cast<RETask *>(this->task);
}


bool
RETaskItem::providesView() const
{
  return true;
}


QWidget *
RETaskItem::createView()
{
  return new RETaskView(this);
}
