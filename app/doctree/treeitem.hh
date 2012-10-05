#ifndef __INA_APP_DOCTREE_TREEITEM_HH__
#define __INA_APP_DOCTREE_TREEITEM_HH__

#include <QVariant>
#include <QModelIndex>


/**
 * Defines a tree-like structure by pointers.
 */
class TreeItem
{
protected:
  TreeItem *_tree_parent;
  QList<TreeItem *> _children;

public:
  virtual TreeItem *getTreeChild(int row) const;

  virtual void setTreeParent(TreeItem *parent);

  virtual TreeItem *getTreeParent() const;

  virtual int getTreeRow() const;

  virtual int indexOfTreeChild(const TreeItem *node) const;

  virtual int getTreeColumnCount() const = 0;

  virtual int getTreeChildCount() const;

  virtual QVariant getTreeData(int column) const = 0;

  virtual void addChild(TreeItem *node);

  virtual void removeChild(TreeItem *node);

protected:
  virtual void resetTree();
};


#endif // __INA_APP_DOCTREE_TREEITEM_HH__
