#include "documenttree.hh"


DocumentTree::DocumentTree(QObject *parent)
  : QAbstractItemModel(parent), TreeItem()
{
  // Pass...
}


void
DocumentTree::resetCompleteTree()
{
  resetTree();
  this->reset();
}


void
DocumentTree::addDocument(DocumentItem *document)
{
  // Check if model is allready in list of open models:
  if (this->_children.contains(document))
  {
    document->deleteLater();
    return;
  }

  // Take ownership of document:
  document->setParent(this);

  // Add document to _children:
  emit this->layoutAboutToBeChanged();
  this->addChild(document);
  emit this->layoutChanged();
}


void
DocumentTree::addTask(DocumentItem *document, TaskItem *task)
{
  emit this->layoutAboutToBeChanged();
  // Add task to document (document takes ownership of task):
  document->addTask(task);
  emit this->layoutChanged();
}


void
DocumentTree::addPlot(TaskItem *task, PlotItem *plot)
{
  emit this->layoutAboutToBeChanged();
  // Add plot to analysis:
  task->addPlot(plot);
  emit this->layoutChanged();
}


void
DocumentTree::removeTask(TaskItem *task)
{
  this->removeItem(task);
}


void
DocumentTree::removeDocument(DocumentItem *document)
{
  this->removeItem(document);
}


void
DocumentTree::removeItem(TreeItem *item)
{
  // Get the item index, parent index and parent item:
  QModelIndex item_index = this->getIndexOf(item);
  QModelIndex parent_index = this->parent(item_index);
  TreeItem *parent_item = item->getTreeParent();

  // Remove from model:
  emit this->layoutAboutToBeChanged();
  this->beginRemoveRows(parent_index, item->getTreeRow(), item->getTreeRow());
  parent_item->removeChild(item);
  this->endRemoveRows();
  emit this->layoutChanged();

  /// \todo This complete reset of the QAbstractItemModel should be replaced by a more
  ///       selective event.
  this->reset();
}

void
DocumentTree::removePlot(PlotItem *plot)
{
  this->removeItem(plot);
}

void
DocumentTree::markForUpdate(TreeItem *item)
{
  QModelIndex idx = this->getIndexOf(item);
  emit this->dataChanged(idx, idx);
}



/* ********************************************************************************************* *
 * Implementation of AbstractTreeModelInterface
 * ********************************************************************************************* */
QModelIndex
DocumentTree::index(int row, int column, const QModelIndex &parent) const
{
  if (! this->hasIndex(row, column, parent))
  {
    return QModelIndex();
  }

  const TreeItem *parentItem = 0;
  if (! parent.isValid()) {
    parentItem = this;
  } else {
    parentItem = static_cast<TreeItem *>(parent.internalPointer());
  }

  TreeItem *childItem = 0;
  if (0 != (childItem = parentItem->getTreeChild(row)))
  {
    return this->createIndex(row, column, childItem);
  }

  return QModelIndex();
}


QModelIndex
DocumentTree::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
  TreeItem *parentItem = childItem->getTreeParent();

  if (parentItem == this)
    return QModelIndex();

  return createIndex(parentItem->getTreeRow(), 0, parentItem);
}


int
DocumentTree::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
  {
    return static_cast<TreeItem *>(parent.internalPointer())->getTreeColumnCount();
  }

  return this->getTreeColumnCount();
}


int
DocumentTree::rowCount(const QModelIndex &parent) const
{
  TreeItem const *parentItem = 0;

  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = this;
  else
    parentItem = static_cast<TreeItem*>(parent.internalPointer());

  return parentItem->getTreeChildCount();
}


QVariant
DocumentTree::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole)
    return QVariant();

  const TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

  return item->getTreeData(index.column());
}


Qt::ItemFlags
DocumentTree::flags(const QModelIndex &index) const
 {
  if (!index.isValid())
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
 }


QVariant
DocumentTree::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return this->getTreeData(section);

  return QVariant();
 }


QModelIndex
DocumentTree::getIndexOf(TreeItem *item) const
{
  QModelIndex parent;

  if (0 != item->getTreeParent())
  {
    parent = this->getIndexOf(item->getTreeParent());
  }

  return this->index(item->getTreeRow(), 0, parent);
}



/* ********************************************************************************************* *
 * Implementation of TreeItem interface
 * ********************************************************************************************* */
TreeItem *
DocumentTree::getTreeParent() const
{
  return 0;
}


int
DocumentTree::getTreeRow() const
{
  return 0;
}

int
DocumentTree::getTreeColumnCount() const
{
  return 1;
}


QVariant
DocumentTree::getTreeData(int column) const
{
  if (0 == column)
  {
    return QVariant("Models");
  }

  return QVariant();
}


