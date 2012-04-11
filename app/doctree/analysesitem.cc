#include "analysesitem.hh"
#include "exception.hh"
#include "../application.hh"

using namespace Fluc;

AnalysesItem::AnalysesItem(QObject *parent) :
  QObject(parent), DocumentTreeItem(), itemLabel("Analyses")
{
  // pass...
}


const QString &
AnalysesItem::getLabel() const
{
  return this->itemLabel;
}


bool
AnalysesItem::tasksRunning()
{
  for (QList<TreeItem *>::iterator iter = this->_children.begin(); iter != this->_children.end(); iter++) {
    TaskItem *item = static_cast<TaskItem *>(*iter);

    switch (item->getState())
    {
    case Task::INITIALIZED:
    case Task::RUNNING:
    case Task::TERMINATING:
      return true;

    default:
      break;
    }
  }

  return false;
}


void
AnalysesItem::addTask(TaskItem *task)
{
  task->setParent(this);
  task->setTreeParent(this);
  this->_children.append(task);
}
