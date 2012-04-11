#ifndef __INA_APP_DOCTREE_TASKITEM_HH__
#define __INA_APP_DOCTREE_TASKITEM_HH__

#include <QObject>
#include <QMenu>

#include "documenttreeitem.hh"
#include "../task.hh"
#include "plotitem.hh"



/**
 * Wraps a task as a TreeItem (@c Wrapper) to be shown in the side-panel of the application.
 *
 * @ingroup gui
 */
class TaskItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

protected:
  /**
   * Holds the task being executed.
   */
  Task *task;

  /**
   * Holds the label of the task. This is just a joint string of the task-named obtained by
   * @c getTaskName and the current progress of the task. The string will be updated each time
   * a @c updateProgress or @c stateChanged event is emmited by the task.
   */
  QString itemLabel;

  /**
   * Holds the context menu.
   */
  QMenu *context_menu;

  /**
   * This action removes the task from the document.
   */
  QAction *removeAct;

  /**
   * This sction terminates the task, if running.
   */
  QAction *stopAct;


public:
  /**
   * Constructs a task wrapper for the given task.
   */
  explicit TaskItem(Task *task, QObject *parent=0);

  virtual ~TaskItem();

  /**
   * Executes the task as a separate thread.
   *
   * This method will return imediately if the task is executed in a separate thread.
   */
  void run();

  /**
   * Returns the state of the task.
   */
  Task::State getState() const;

  /**
   * Returns the label of the tree item, implements the @c Wrapper interface.
   */
  virtual const QString &getLabel() const;

  /**
   * Returns the task.
   */
  Task *getTask();

  virtual bool providesContextMenu() const;

  virtual void showContextMenu(const QPoint &global_pos);

  void addPlot(PlotItem *plot);


private slots:
  /**
   * Will be connected to the progress signal of the task
   */
  void onProgress();

  void onStateChanged();

  void terminateTask();

  void removeTask();
};

#endif
