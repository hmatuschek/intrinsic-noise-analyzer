#include "tableitem.hh"



TableItem::TableItem(TableWrapper *table_wrapper, const QString &label, QObject *parent)
  : QObject(parent), DocumentTreeItem(), table_wrapper(table_wrapper), label(label)
{
  // Take ownership of the table-wrapper
  table_wrapper->setParent(this);
}


const QString &
TableItem::getLabel() const
{
  return this->label;
}
