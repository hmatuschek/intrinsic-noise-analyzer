#include "documenttreeitem.hh"
#include "../models/application.hh"
#include "documentitem.hh"
#include "documenttree.hh"


DocumentTreeItem::DocumentTreeItem()
  : TreeItem()
{
  // pass...
}


bool
DocumentTreeItem::providesView() const {
  return false;
}


QWidget *
DocumentTreeItem::createView() {
  return 0;
}


bool
DocumentTreeItem::providesContextMenu() const {
  return false;
}


void
DocumentTreeItem::showContextMenu(const QPoint &global_pos) {
  Q_UNUSED(global_pos);
}


int
DocumentTreeItem::getTreeColumnCount() const {
  return 1;
}


QVariant
DocumentTreeItem::getTreeData(int column) const {
  if (0 != column) { return QVariant(); }
  return QVariant(this->getLabel());
}

DocumentItem *
DocumentTreeItem::document() {
  // If the parent item is THE document tree, this must be a DocumentItem
  if (0 != dynamic_cast<DocumentTree *>(_tree_parent)) {
    return dynamic_cast<DocumentItem *>(this);
  }
  return dynamic_cast<DocumentTreeItem *>(_tree_parent)->document();
}
