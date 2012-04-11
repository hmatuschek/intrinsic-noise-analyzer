#ifndef __INA_APP_DOCTREE_TABLEITEM_HH__
#define __INA_APP_DOCTREE_TABLEITEM_HH__

#include <QObject>
#include "documenttreeitem.hh"
#include "../models/tablewrapper.hh"


/**
 * This class represents a TableWrapper (holding a table) in the application tree.
 *
 * Such a table-item can be attached to an @c TaskWrapper to hold results of an analysis.
 *
 * @ingroup gui
 */
class TableItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT


protected:
  /**
   * Holds the @c TableWrapper instance itself holding the actual table.
   */
  TableWrapper *table_wrapper;

  /**
   * Holds the label of the table item.
   */
  QString label;


public:
  /**
   * Constructs a new table item from the given @c TableWrapper item.
   */
  TableItem(TableWrapper *table_wrapper, const QString &label, QObject *parent=0);

  /* ******************************************************************************************* *
   * Implementation of the Wrapper interface.
   * ******************************************************************************************* */
  virtual const QString &getLabel() const;
};


#endif
