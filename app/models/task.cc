#include "task.hh"
#include "exception.hh"
#include <iostream>
#include <stdexcept>
#include <utils/logger.hh>



Task::Task(QObject *parent)
  : QThread(parent), progress(0.0), state(Task::INITIALIZED), error_message()
{
  // Connect signals:
  QObject::connect(this, SIGNAL(terminated()), this, SLOT(taskTerminated()));
}


Task::~Task()
{
  // Pass...
}


void
Task::run()
{
  // Enable termination by user:
  QThread::setTerminationEnabled(true);

  // Try to catch all Fluc::Exception instances, avoids a crash of the application if an error
  // occures.
  try {
    start_time = clock();
    process();
  } catch (iNA::Exception &err) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
    message << "Task: Caught exception: " << err.what();
    iNA::Utils::Logger::get().log(message);
    error_message.setTitle("Exception during analysis.");
    error_message.setDetails(err.what());
    setState(Task::ERROR);
  } catch (std::runtime_error &err) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Task:: Caught std::runtime_error: " << err.what();
    error_message.setTitle("Runtime exception during analysis.");
    error_message.setDetails(err.what());
    setState(Task::ERROR);
  }
}


void
Task::setProgress(double progress)
{
  this->progress = progress;

  emit this->updateProgress();
}


double
Task::getProgress() const
{
  return this->progress;
}


void
Task::setState(State state)
{
  this->state = state;

  emit this->stateChanged();
}


void
Task::setState(State state, const QString &message)
{
  this->error_message.setDetails(message);
  this->setState(state);
}


Task::State
Task::getState() const
{
  return this->state;
}


const TaskError &
Task::getErrorMessage() const
{
  return this->error_message;
}


double
Task::getElapsedTime()
{
  clock_t end = clock();

  double dt = end-this->start_time;
  dt /= CLOCKS_PER_SEC;

  return dt;
}


void
Task::taskTerminated()
{
  this->setState(Task::ERROR, "Task was terminated by user.");
}
