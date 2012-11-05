#include "taskitem.hh"
#include <iostream>

#include "../models/application.hh"
#include "analysesitem.hh"


TaskItem::TaskItem(Task *task, QObject *parent)
  : QObject(parent), DocumentTreeItem(), task(task), itemLabel()
{
  // Take ownership of task:
  task->setParent(this);

  // Construct actions:
  this->context_menu = new QMenu();
  this->removeAct = new QAction(tr("Remove analysis"), this->context_menu);
  this->stopAct = new QAction(tr("Stop analysis"), this->context_menu);

  // Assemble menu:
  this->context_menu->addAction(this->stopAct);
  this->context_menu->addAction(this->removeAct);

  // Enable/Disable action by task state:
  switch (this->task->getState())
  {
  case Task::DONE:
  case Task::ERROR:
    this->removeAct->setEnabled(true);
    this->stopAct->setEnabled(false);
    break;

  case Task::INITIALIZED:
  case Task::RUNNING:
    this->removeAct->setEnabled(false);
    this->stopAct->setEnabled(true);
    break;

  case Task::TERMINATING:
    this->removeAct->setEnabled(false);
    this->stopAct->setEnabled(false);
    break;
  }

  // Connect to tasks "progress" signal:
  QObject::connect(task, SIGNAL(updateProgress()), this, SLOT(onProgress()));
  QObject::connect(task, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));

  // Connect to menu events:
  QObject::connect(this->stopAct, SIGNAL(triggered()), this, SLOT(terminateTask()));
  QObject::connect(this->removeAct, SIGNAL(triggered()), this, SLOT(removeTask()));
}


TaskItem::~TaskItem()
{
  // Hide and clean menu, and mark it for deletion later on:
  this->context_menu->hide();
  this->context_menu->clear();
  this->context_menu->deleteLater();
}


void
TaskItem::run()
{
  this->task->start();
}


Task::State
TaskItem::getState() const
{
  return this->task->getState();
}


const QString &
TaskItem::getLabel() const
{
  return this->itemLabel;
}


Task *
TaskItem::getTask()
{
  return this->task;
}


bool
TaskItem::providesContextMenu() const
{
  return true;
}


void
TaskItem::showContextMenu(const QPoint &global_pos)
{
  this->context_menu->popup(global_pos);
}


void
TaskItem::addPlot(PlotItem *plot)
{
  plot->setParent(this);
  this->addChild(plot);
}


void
TaskItem::onProgress()
{
  // Update label:
  this->itemLabel = QString("%1 (%2 \%)").arg(this->task->getLabel()).arg(int(this->task->getProgress()*100));

  // Mark item for update
  Application::getApp()->docTree()->markForUpdate(this);
}


void
TaskItem::onStateChanged()
{
  // Enable/Disable action by task state:
  switch (this->task->getState())
  {
  case Task::DONE:
  case Task::ERROR:
    this->removeAct->setEnabled(true);
    this->stopAct->setEnabled(false);
    break;

  case Task::INITIALIZED:
  case Task::RUNNING:
    this->removeAct->setEnabled(false);
    this->stopAct->setEnabled(true);
    break;

  case Task::TERMINATING:
    this->removeAct->setEnabled(false);
    this->stopAct->setEnabled(false);
    break;
  }

  if (Task::ERROR == this->task->getState())
  {
    // Update label:
    this->itemLabel = QString("%1 (error)").arg(this->task->getLabel());
  }

  if (Task::TERMINATING == this->task->getState())
  {
    // Update label:
    this->itemLabel = QString("%1").arg(this->task->getLabel());
  }

  if (Task::DONE == this->task->getState())
  {
    // Update label:
    this->itemLabel = QString("%1 (done)").arg(this->task->getLabel());
  }

  // Mark item for update
  Application::getApp()->docTree()->markForUpdate(this);
}


void
TaskItem::terminateTask()
{
  if (Task::RUNNING == this->task->getState() ||
      Task::INITIALIZED == this->task->getState())
    this->task->setState(Task::TERMINATING);
}


void
TaskItem::removeTask()
{
  if (Task::DONE != this->task->getState() &&
      Task::ERROR != this->task->getState()) {
    return;
  }

  // Remove item from document tree:
  Application::getApp()->docTree()->removeTask(this);

  // Mark object for deletion:
  this->deleteLater();
}



