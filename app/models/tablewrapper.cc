#include "tablewrapper.hh"


TableWrapper::TableWrapper(Table *table, QObject *parent) :
  QAbstractTableModel(parent), table(table)
{
  // Pass...
}


int
TableWrapper::rowCount(const QModelIndex &parent) const
{
  return this->table->getNumRows();
}


int
TableWrapper::columnCount(const QModelIndex &parent) const
{
  return this->table->getNumColumns();
}


QVariant
TableWrapper::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || Qt::DisplayRole != role ||
      size_t(index.row()) >= this->table->getNumRows() ||
      size_t(index.column()) >= this->table->getNumColumns())
  {
    return QVariant();
  }

  return QVariant((*(this->table))(index.row(), index.column()));
}


QVariant
TableWrapper::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || (size_t(section) >= this->table->getNumColumns()) ||
      Qt::Horizontal != orientation)
  {
    return QVariant();
  }

  return QVariant(this->table->getColumnName(section));
}
