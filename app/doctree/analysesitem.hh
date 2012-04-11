#ifndef __INA_APP_DOCTREE_ANALYSES_HH__
#define __INA_APP_DOCTREE_ANALYSES_HH__

#include <QObject>
#include "documenttreeitem.hh"
#include "taskitem.hh"


/**
 * Represents a list of analyses/tasks.
 *
 * @ingroup gui
 */
class AnalysesItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT


public:
  explicit AnalysesItem(QObject *parent = 0);

  virtual const QString &getLabel() const;

  /**
   * Returns true, if there are any running tasks.
   */
  bool tasksRunning();


public slots:
  void addTask(TaskItem *task);


private:
  QString itemLabel;
};

#endif // __INA_APP_DOCTREE_ANALYSES_HH__
