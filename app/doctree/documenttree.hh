#ifndef __INA_APP_WRAPPER_DOCUMENTTREE_HH__
#define __INA_APP_WRAPPER_DOCUMENTTREE_HH__

#include <QAbstractItemModel>
#include "../doctree/treeitem.hh"

#include "documentitem.hh"
#include "taskitem.hh"
#include "plotitem.hh"


class DocumentTree :  public QAbstractItemModel, public TreeItem
{
  Q_OBJECT

public:
  DocumentTree(QObject *parent=0);

  void resetCompleteTree();

  virtual void addDocument(DocumentItem *document);

  virtual void addTask(DocumentItem *document, TaskItem *task);

  virtual void addPlot(TaskItem *task, PlotItem *plot);

  virtual void removeTask(TaskItem *task);

  virtual void removeDocument(DocumentItem *document);

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
  QVariant data(const QModelIndex &index, int role) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const;

  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const;

  QModelIndex parent(const QModelIndex &index) const;

  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  int columnCount(const QModelIndex &parent = QModelIndex()) const;


private:
  void removeItem(TreeItem *item);
};

#endif // DOCUMENTTREE_HH
