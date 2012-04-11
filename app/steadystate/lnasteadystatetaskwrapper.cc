#include "lnasteadystatetaskwrapper.hh"
#include "lnasteadystateview.hh"



LNASteadyStateTaskWrapper::LNASteadyStateTaskWrapper(LNASteadyStateTask *task, QObject *parent)
  : TaskItem(task, parent), spectrum_wrapper(&(task->getSpectrum()))
{
}


LNASteadyStateTask *
LNASteadyStateTaskWrapper::getSteadyStateTask()
{
  return static_cast<LNASteadyStateTask *>(this->task);
}


bool
LNASteadyStateTaskWrapper::providesView() const
{
  return true;
}


QWidget *
LNASteadyStateTaskWrapper::createView()
{
  return new LNASteadyStateView(this, 0);
}


TableWrapper *
LNASteadyStateTaskWrapper::getSpectrum()
{
  return &(this->spectrum_wrapper);
}
