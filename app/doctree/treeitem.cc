#include "treeitem.hh"
#include <iostream>



TreeItem *
TreeItem::getTreeChild(int row) const
{
  return this->_children.value(row);
}


void
TreeItem::setTreeParent(TreeItem *parent)
{
  this->_tree_parent = parent;
}


TreeItem *
TreeItem::getTreeParent() const
{
  return this->_tree_parent;
}


int
TreeItem::getTreeRow() const
{
  if (0 == this->_tree_parent)
    return 0;

  return this->_tree_parent->indexOfTreeChild(this);
}


int
TreeItem::indexOfTreeChild(const TreeItem *node) const
{
  return this->_children.indexOf(const_cast<TreeItem *>(node));
}


int
TreeItem::getTreeChildCount() const
{
  return this->_children.size();
}


void
TreeItem::addChild(TreeItem *node)
{
  node->setTreeParent(this);
  this->_children.append(node);
}


void
TreeItem::removeChild(TreeItem *node)
{
  if (! this->_children.removeOne(node))
  {
    std::cerr << "OOps can not remove tree-item " << node << " not in list?" << std::endl;
  }
}


void
TreeItem::resetTree()
{
  for (QList<TreeItem *>::iterator item=_children.begin(); item!=_children.end(); item++) {
    (*item)->resetTree();
  }
}
