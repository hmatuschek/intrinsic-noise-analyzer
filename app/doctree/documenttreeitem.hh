#ifndef __INA_APP_DOCTREE_DOCUMENTTREEITEM_HH__
#define __INA_APP_DOCTREE_DOCUMENTTREEITEM_HH__

#include "treeitem.hh"

// Forward declaration
class DocumentItem;

/** Basic interface for all wrappers, holding some data organized in the tree-like structure of the
 * application.
 *
 * This class provides a much simpler interface as the @c TreeItem class, a derived class has only
 * to implement the @c getLabel method.
 *
 * @ingroup gui */
class DocumentTreeItem : public TreeItem
{
public:
  /** Constructor. */
  DocumentTreeItem();

  /** This method can be overridden to signal, that there is a view for the item.
   * By default this method returns false, indicateing that there is not view for the item. */
  virtual bool providesView() const;

  /** This method can be overridden to implement the instantiation of a view widget for the item.
   * By default this method returns 0. */
  virtual QWidget *createView();

  /** This method returns true, if the item has a context-menu. */
  virtual bool providesContextMenu() const;

  /** Should be overridden and should return a weak reference to a QMenu instance, being the
   * context menu of the item. */
  virtual void showContextMenu(const QPoint &global_pos);

  /** This method must be implemented by all @c TreeItem classes to return a label for the item
   * in the application tree view. */
  virtual const QString &getLabel() const = 0;

  /** Returns the document item (root) of the this item. */
  virtual DocumentItem *document();


  /* ******************************************************************************************* *
   * Implementation of the TreeItem interface.
   * ******************************************************************************************* */

  /** Implements the @c TreeItem interface.
   * Returns the number of columns (1). */
  virtual int getTreeColumnCount() const;

  /** Implements the @c TreeItem interface.
   * Returns the label. */
  virtual QVariant getTreeData(int column) const;
};


#endif // __INA_APP_DOCTREE_DOCUMENTTREEITEM_HH__
