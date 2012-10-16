#include "steadystatetaskwrapper.hh"
#include "steadystateview.hh"



SteadyStateTaskWrapper::SteadyStateTaskWrapper(SteadyStateTask *task, QObject *parent)
  : TaskItem(task, parent), spectrum_wrapper(&(task->getSpectrum()))
{
}


SteadyStateTask *
SteadyStateTaskWrapper::getSteadyStateTask()
{
  return static_cast<SteadyStateTask *>(this->task);
}


bool
SteadyStateTaskWrapper::providesView() const
{
  return true;
}


QWidget *
SteadyStateTaskWrapper::createView()
{
  return new SteadyStateView(this, 0);
}


TableWrapper *
SteadyStateTaskWrapper::getSpectrum()
{
  return &(this->spectrum_wrapper);
}
