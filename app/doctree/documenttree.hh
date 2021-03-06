#ifndef __INA_APP_WRAPPER_DOCUMENTTREE_HH__
#define __INA_APP_WRAPPER_DOCUMENTTREE_HH__

#include <QAbstractItemModel>
#include "../doctree/treeitem.hh"

// Forward declaraions:
class DocumentItem;
class TaskItem;
class PlotItem;
class ReactionItem;


class DocumentTree :  public QAbstractItemModel, public TreeItem
{
  Q_OBJECT

public:
  DocumentTree(QObject *parent=0);

  void resetCompleteTree();

  virtual void addDocument(DocumentItem *document);
  virtual void addTask(DocumentItem *document, TaskItem *task);
  virtual void addPlot(TaskItem *task, PlotItem *plot);
  virtual void addReaction(DocumentItem *document, ReactionItem *reaction);
  virtual void updateReactions(DocumentItem *document);
  virtual void removeTask(TaskItem *task);
  virtual void removeDocument(DocumentItem *document);
  virtual void removePlot(PlotItem *plot);
  virtual void removeReaction(ReactionItem *item);
  virtual void markForUpdate(TreeItem *item);

  /* ******************************************************************************************* *
   * Implementation of the TreeItem interface:
   * ******************************************************************************************* */
  QModelIndex getIndexOf(TreeItem *item) const;
  virtual TreeItem *getTreeParent() const;
  virtual int getTreeRow() const;
  virtual int getTreeColumnCount() const;
  virtual QVariant getTreeData(int column) const;


  /* ******************************************************************************************* *
   * Implementation of the QAbstractItemModel interface
   * ******************************************************************************************* */
  virtual QVariant data(const QModelIndex &index, int role) const;

  virtual Qt::ItemFlags flags(const QModelIndex &index) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role = Qt::DisplayRole) const;

  virtual QModelIndex index(int row, int column,
                            const QModelIndex &parent = QModelIndex()) const;

  virtual QModelIndex parent(const QModelIndex &index) const;

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:
  /** Gets emitted if an item gets activated automatically, i.e. a Task is added to the
   * doctree. In this case its item gets activated automatically and the view should
   * react on that event by selecing that activated item. */
  void autoView(const QModelIndex &item);

private:
  void removeItem(TreeItem *item);
};

#endif // DOCUMENTTREE_HH
