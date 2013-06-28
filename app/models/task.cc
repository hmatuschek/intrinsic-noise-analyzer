#include "task.hh"
#include "exception.hh"
#include <iostream>
#include <stdexcept>
#include <utils/logger.hh>



Task::Task(QObject *parent)
  : QThread(parent), _progress(0.0), _state(Task::INITIALIZED), _error_message()
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
    _start_time = time(0);
    _current_time = time(0);
    process();
  } catch (iNA::Exception &err) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
    message << "Task: Caught exception: " << err.what();
    iNA::Utils::Logger::get().log(message);
    _error_message.setTitle("Exception during analysis.");
    _error_message.setDetails(err.what());
    setState(Task::ERROR);
  } catch (std::runtime_error &err) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Task:: Caught std::runtime_error: " << err.what();
    iNA::Utils::Logger::get().log(message);
    _error_message.setTitle("Runtime exception during analysis.");
    _error_message.setDetails(err.what());
    setState(Task::ERROR);
  } catch (std::domain_error &err) {
      iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
      message << "Task:: Caught std::domain_error: " << err.what();
      iNA::Utils::Logger::get().log(message);
      _error_message.setTitle("Domain error during analysis.");
      _error_message.setDetails(err.what());
      setState(Task::ERROR);
  }

}


void
Task::setProgress(double progress)
{
  this->_progress = progress;
  _current_time = time(0);
  emit this->updateProgress();
}


double
Task::getProgress() const
{
  return this->_progress;
}


void
Task::setState(State state)
{
  this->_state = state;
  emit this->stateChanged();
}


void
Task::setState(State state, const QString &message)
{
  this->_error_message.setDetails(message);
  this->setState(state);
}


Task::State
Task::getState() const
{
  return this->_state;
}


const TaskError &
Task::getErrorMessage() const
{
  return this->_error_message;
}


double
Task::getElapsedTime()
{
  double dt = difftime(this->_current_time, this->_start_time);
  return dt;
}


void
Task::taskTerminated()
{
  this->setState(Task::ERROR, "Task was terminated by user.");
}
