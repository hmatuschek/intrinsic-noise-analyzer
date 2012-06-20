#ifndef TASKVIEW_HH
#define TASKVIEW_HH

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

#include "../task.hh"
#include "../doctree/taskitem.hh"
#include "taskerrorwidget.hh"


/**
 * Common view of a task.
 *
 * This view provides some basic widgets for any task: A progress-view, showing the progress of
 * the task. A error view, showing some details of an error occurred during the calculations. The
 * user must just provide a 'result-view', showing the results of a task, once it is completed.
 *
 * The @c TaskView listens for the state and progress event of the task and switches between the
 * different views on task-state changes.
 *
 * @ingroup gui
 */
class TaskView : public QWidget
{
  Q_OBJECT

protected:
  /**
   * A weak reference to the task-item.
   */
  TaskItem *task_item;


protected:
  /**
   * Constructs a basic-task view for the given task-item.
   */
  TaskView(TaskItem *task_item, QWidget *parent = 0);


public:
  /**
   * Destructor.
   */
  virtual ~TaskView();


protected:
  /**
   * (Re-) Sets the default progress widget.
   */
  void setProgressWidget(QWidget *widget);

  /**
   * Sets the default result-widget.
   */
  void setResultWidget(QWidget *widget);

  /**
   * Resets the default error-widget.
   */
  void setErrorWidget(QWidget *widget);


protected slots:
  /**
   * Processes a state-change of the task.
   */
  void taskStateChanged();

  /**
   * Callback, if the task gets deleted.
   */
  void taskDestroyed();


private:
  /**
   * Private helper function to setup widgets.
   */
  void initWidgets();

private:
  /**
   * Title widget.
   */
  QLabel *title;

  /**
   * Layout holding all widets.
   */
  QStackedWidget *stack;
};



/**
 * Default widget, being shown if a task is about to terminate.
 *
 * @ingroup gui
 */
class TaskTerminatingWidget : public QLabel
{
  Q_OBJECT

public:
  /**
   * Constructor.
   */
  explicit TaskTerminatingWidget(QWidget *parent=0);
};



/**
 * Default progress widget for a task.
 *
 * @ingroup gui
 */
class TaskProgressWidget : public QWidget
{
  Q_OBJECT

protected:
  /**
   * Holds a weak reference to the taks-item.
   */
  TaskItem *task_item;

  /** Holds a timer to update the state and time of a task. */
  QTimer *_updateTimer;

public:
  /**
   * Cosntructor for the given task-item.
   */
  explicit TaskProgressWidget(TaskItem *task_item, QWidget *parent=0);


protected slots:
  /**
   * Callback if the task made some progress.
   */
  void taskProgress();

  /**
   * Callback if the task state has changed.
   */
  void taskStateChanged();

  /**
   * Callback to terminate task.
   */
  void stopAnalysis();


private:
  /**
   * Tiny helper function to format the elapsed and remaining time.
   */
  QString formatTime(double sec);


private:
  /**
   * The progress bar.
   */
  QProgressBar *progress_bar;

  /**
   * Label to show elapsed time.
   */
  QLabel *time_elapsed;

  /**
   * Label to show remaining time.
   */
  QLabel *time_remain;
};



#endif // TASKVIEW_HH
