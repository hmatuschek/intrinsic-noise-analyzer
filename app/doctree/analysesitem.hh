#ifndef __INA_APP_DOCTREE_ANALYSES_HH__
#define __INA_APP_DOCTREE_ANALYSES_HH__

#include <QObject>
#include "documenttreeitem.hh"

class TaskItem;

/** Represents a list of analyses/tasks.
 * @ingroup gui */
class AnalysesItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

public:
  explicit AnalysesItem(QObject *parent = 0);

  /** Returns the item label. */
  virtual const QString &getLabel() const;
  /** Returns true, if there are any running tasks. */
  bool tasksRunning();


public slots:
  /** Adds a task to the list of analyses. */
  void addTask(TaskItem *task);

private:
  /** Holds the item label. */
  QString _itemLabel;
};

#endif // __INA_APP_DOCTREE_ANALYSES_HH__
