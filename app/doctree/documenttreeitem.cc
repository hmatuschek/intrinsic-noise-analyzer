#include "documenttreeitem.hh"
#include "../models/application.hh"

DocumentTreeItem::DocumentTreeItem()
  : TreeItem()
{
  // pass...
}


bool
DocumentTreeItem::providesView() const
{
  return false;
}


QWidget *
DocumentTreeItem::createView()
{
  return 0;
}


bool
DocumentTreeItem::providesContextMenu() const
{
  return false;
}


void
DocumentTreeItem::showContextMenu(const QPoint &global_pos)
{
  // Pass...
}


int
DocumentTreeItem::getTreeColumnCount() const
{
  return 1;
}


QVariant
DocumentTreeItem::getTreeData(int column) const
{
  if (0 != column)
  {
    return QVariant();
  }

  return QVariant(this->getLabel());
}
