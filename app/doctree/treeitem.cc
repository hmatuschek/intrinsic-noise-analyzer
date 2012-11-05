#include "treeitem.hh"
#include <iostream>
#include <utils/logger.hh>


TreeItem::TreeItem()
  : _tree_parent(0)
{
  // Pass...
}


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
  if (! this->_children.removeOne(node)) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "OOps can not remove tree-item " << node << " not in list?";
    iNA::Utils::Logger::get().log(message);
  }
}


void
TreeItem::resetTree()
{
  for (QList<TreeItem *>::iterator item=_children.begin(); item!=_children.end(); item++) {
    (*item)->resetTree();
  }
}
