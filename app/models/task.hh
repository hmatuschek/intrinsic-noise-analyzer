#ifndef __INA_APP_MODELS_TASK_HH__
#define __INA_APP_MODELS_TASK_HH__

#include <QObject>
#include <QThread>
#include <time.h>

#include "taskerror.hh"


/**
 * Pure virtual class to perform calculations in separate threads.
 *
 * This class provides some mechanisms to signal the appliction about the state and progress of
 * the task. Use the method @c setState to inform the application about state-changes of the task,
 * the method @c setProgress can be used to inform the application about the current progress
 * of the task.
 *
 * @ingroup gui
 */
class Task : public QThread
{
  Q_OBJECT

public:
  /** Defines the possible states of a task. */
  typedef enum {
    INITIALIZED,  /// < The task was initialized, and will be started.
    RUNNING,      /// < The task is running.
    DONE,         /// < The task is finished with success.
    ERROR,        /// < The task is finished with an error.
    TERMINATING   /// < The task is terminating, once finished the state will be ERROR.
  } State;


protected:
  /** Represents the progress of the task, a value between 0 and 1. */
  double progress;
  /** Holds the current state of the task. */
  State state;
  /** Holds an optional error message if the analysis fails. */
  TaskError error_message;
  /** Holds the clock measureing the elapsed time. */
  clock_t start_time;

protected:
  /** Constructs a task, is hidden to avoid direct instantiation. */
  explicit Task(QObject *parent=0);

  /** Destructor. */
  virtual ~Task();

  /** Reimplements @c QThread::run. Catches all @c Fluc::Exception. Calls the pure virtual method
   * @c process. */
  virtual void run();

public:
  /** Returns the current progress of the task. */
  double getProgress() const;

  /** Returns the current state of the task. */
  State getState() const;

  /** Updates the state of the task and emmits the updateState signal. */
  void setState(State state);

  /** Get the time in seconds since process start. */
  double getElapsedTime();

  /** Retunrs the error message. */
  const TaskError &getErrorMessage() const;

  /** Returns a QString of the task-label. This label is shown in the task list and
   * above the "TaskView". */
  virtual QString getLabel() = 0;

protected:
  /** Sets the progress of the state and emmits the updateProgress signal. */
  void setProgress(double progress);

  /** Updates the state of the task/analysis and sets an error-message. */
  void setState(State state, const QString &message);

  /** Needs to be implemented to perform the actual analysis task. */
  virtual void process() = 0;

signals:
  /** This signal can be emmited by the working thread to update its process. */
  void updateProgress();

  /** Will be emmitted, if the task is done successfully. */
  void stateChanged();


protected slots:
  /** Connected to the threads terminated() signal, sets an error message. */
  void taskTerminated();
};

#endif // __INA_APP_MODELS_TASK_HH__
