#include "documenttree.hh"
#include "documentitem.hh"
#include "analysesitem.hh"
#include "taskitem.hh"
#include "plotitem.hh"
#include "reactionitem.hh"
#include "reactionsitem.hh"
#include "../models/application.hh"

#include <QDebug>



DocumentTree::DocumentTree(QObject *parent)
  : QAbstractItemModel(parent), TreeItem()
{
  // Pass...
}


void
DocumentTree::resetCompleteTree() {
  resetTree();
  this->reset();
}


void
DocumentTree::addDocument(DocumentItem *document) {
  beginInsertRows(QModelIndex(), getTreeChildCount(), getTreeChildCount());
  addChild(document);
  endInsertRows();
}


void
DocumentTree::addTask(DocumentItem *document, TaskItem *task) {
  // Add task to document (document takes ownership of task):
  QModelIndex analyses_index = getIndexOf(document->analysesItem());
  size_t num_analyses = document->numAnalyses();
  beginInsertRows(analyses_index, num_analyses, num_analyses);
  document->addTask(task);
  endInsertRows();
  Application::getApp()->itemSelected(task);
}


void
DocumentTree::addPlot(TaskItem *task, PlotItem *plot) {
  // Add plot to analysis:
  QModelIndex task_index = getIndexOf(task);
  size_t num_plots = task->getTreeChildCount();
  beginInsertRows(task_index, num_plots, num_plots);
  task->addPlot(plot);
  endInsertRows();
  Application::getApp()->itemSelected(plot);
}

void
DocumentTree::addReaction(DocumentItem *document, ReactionItem *reaction) {
  // Add a reaction to the model addressed by the given document:
  ReactionsItem *reac_item = document->reactionsItem();
  QModelIndex reactions_index = getIndexOf(reac_item);
  size_t num_reacts = reac_item->getTreeChildCount();
  beginInsertRows(reactions_index, num_reacts, num_reacts);
  reac_item->addReaction(reaction);
  endInsertRows();
}

void
DocumentTree::removeTask(TaskItem *task) {
  this->removeItem(task);
}


void
DocumentTree::removeDocument(DocumentItem *document) {
  beginRemoveRows(QModelIndex(), document->getTreeRow(), document->getTreeRow());
  removeChild(document);
  endRemoveRows();
}


void
DocumentTree::removeItem(TreeItem *item)
{
  // Get the item index, parent index and parent item:
  QModelIndex item_index = this->getIndexOf(item);
  QModelIndex parent_index = this->parent(item_index);
  TreeItem *parent_item = item->getTreeParent();

  // Remove from model:
  beginRemoveRows(parent_index, item->getTreeRow(), item->getTreeRow());
  parent_item->removeChild(item);
  endRemoveRows();
}

void
DocumentTree::removePlot(PlotItem *plot) {
  removeItem(plot);
}

void
DocumentTree::removeReaction(ReactionItem *item) {
  removeItem(item);
}

void
DocumentTree::markForUpdate(TreeItem *item) {
  QModelIndex idx = this->getIndexOf(item);
  emit this->dataChanged(idx, idx);
}



/* ********************************************************************************************* *
 * Implementation of AbstractTreeModelInterface
 * ********************************************************************************************* */
QModelIndex
DocumentTree::index(int row, int column, const QModelIndex &parent) const
{
  const TreeItem *parentItem = 0;
  if (! parent.isValid()) { parentItem = this; }
  else { parentItem = static_cast<TreeItem *>(parent.internalPointer()); }

  TreeItem *childItem = 0;
  if (0 != (childItem = parentItem->getTreeChild(row))) {
    return createIndex(row, column, (void *)childItem);
  }

  return QModelIndex();
}


QModelIndex
DocumentTree::parent(const QModelIndex &index) const {
  if (!index.isValid()) { return QModelIndex(); }
  TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
  TreeItem *parentItem = childItem->getTreeParent();
  if (parentItem == this) { return QModelIndex(); }
  return createIndex(parentItem->getTreeRow(), 0, (void *)parentItem);
}


int
DocumentTree::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return static_cast<TreeItem *>(parent.internalPointer())->getTreeColumnCount();
  }
  return this->getTreeColumnCount();
}


int
DocumentTree::rowCount(const QModelIndex &parent) const {
  TreeItem const *parentItem = 0;
  if (parent.column() > 0) { return 0; }
  if (!parent.isValid()) { parentItem = this; }
  else { parentItem = static_cast<TreeItem*>(parent.internalPointer()); }
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
  if (item == this) { return QModelIndex(); }

  if (0 != item->getTreeParent()) {
    parent = this->getIndexOf(item->getTreeParent());
  }
  int row = item->getTreeRow();
  return DocumentTree::index(row, 0, parent);
}



/* ********************************************************************************************* *
 * Implementation of TreeItem interface
 * ********************************************************************************************* */
TreeItem *
DocumentTree::getTreeParent() const{
  return 0;
}


int
DocumentTree::getTreeRow() const {
  return 0;
}

int
DocumentTree::getTreeColumnCount() const {
  return 1;
}


QVariant
DocumentTree::getTreeData(int column) const {
  if (0 == column) { return QVariant(tr("Models")); }
  return QVariant();
}


