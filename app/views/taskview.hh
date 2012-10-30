#ifndef __INA_APP_VIEWS_TASKVIEW_HH__
#define __INA_APP_VIEWS_TASKVIEW_HH__

#include <QWidget>
#include <QStackedWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>

#include "../models/task.hh"
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
  /** Can be overridden by specialized classes to return a specialized progress widget for the
   * task. */
  virtual QWidget *createProgressWidget(TaskItem *task_item);

  /** Can be overridden by specialized classes to return a specialized result widget for the
   * task. */
  virtual QWidget *createResultWidget(TaskItem *task_item) = 0;

  /** Can be overridden by specialized classes to return a specialized error widget for the
   * task. */
  virtual QWidget *createErrorWidget(TaskItem *task_item);

  /** Can be overridden by specialized classes to return a specialized terminating widget for the
   * task. */
  virtual QWidget *createTerminatingWidget(TaskItem *task_item);


protected:
  /** Replaces the main widget. */
  void setMainWidget(QWidget *widget);


protected slots:
  /**
   * Processes a state-change of the task, replaces the main widget.
   */
  void taskStateChanged();

  /**
   * Callback, if the task gets deleted.
   */
  void taskDestroyed();


private:
  /**
   * Title widget.
   */
  QLabel *title;

  /**
   * Layout holding all widets.
   */
  QWidget *current_main_widget;
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



#endif // __INA_APP_VIEWS_TASKVIEW_HH__
