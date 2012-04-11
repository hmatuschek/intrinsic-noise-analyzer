#ifndef TASKERRORWIDGET_HH
#define TASKERRORWIDGET_HH

#include <QWidget>
#include <QLabel>

#include "../task.hh"
#include "../models/taskerror.hh"
#include "../doctree/taskitem.hh"


/**
 * A default widget shown to the user, if the a task fails.
 *
 * Corresponding model: @c TaksError
 *
 * @ingroup gui
 */
class TaskErrorWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * Constructs an empty error widget.
   */
  explicit TaskErrorWidget(TaskItem *task_wrapper, QWidget *parent = 0);


public slots:
  /**
   * Sets the error message and title using a @c TaskError instance.
   */
  void setError(const TaskError &error);


private slots:
  /**
   * Connected to the stateChanged signal of the task.
   */
  void taskStateChanged();


private:
  /**
   * Holds the task instance.
   */
  TaskItem *task_wrapper;

  /**
   * Holds the title label.
   */
  QLabel *title;

  /**
   * Holds the details text.
   */
  QLabel *details;
};

#endif // TASKERRORWIDGET_HH
