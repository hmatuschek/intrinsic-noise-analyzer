#include "ssataskwrapper.hh"
#include "ssaview.hh"


SSATaskWrapper::SSATaskWrapper(SSATask *task, QObject *parent)
  : TaskItem(task, parent)
{
  this->table_wrapper = new TableWrapper(&(task->getTimeSeries()));
}


bool
SSATaskWrapper::providesView() const
{
  return true;
}


QWidget *
SSATaskWrapper::createView()
{
  return new SSATaskView(this, 0);
}


TableWrapper *
SSATaskWrapper::getTimeSeries()
{
  return this->table_wrapper;
}


SSATask *
SSATaskWrapper::getSSATask()
{
  return static_cast<SSATask *>(this->task);
}
