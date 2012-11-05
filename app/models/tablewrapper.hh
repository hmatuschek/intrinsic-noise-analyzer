#ifndef __INA_APP_MODELS_TABLEWRAPPER_HH__
#define __INA_APP_MODELS_TABLEWRAPPER_HH__

#include <QAbstractTableModel>
#include "timeseries.hh"


/**
 * Implements the QAbstractTableModel for a @c Table instance. Allows to show the table (timeseries)
 * in a QTable view.
 *
 * @ingroup gui
 */
class TableWrapper : public QAbstractTableModel
{
  Q_OBJECT

protected:
  /**
   * Holds a weak reference to the table to wrap.
   */
  Table *table;

public:
  /**
   * Constructs a table wrapper around the given table.
   */
  explicit TableWrapper(Table *table, QObject *parent = 0);

  virtual int rowCount(const QModelIndex &parent) const;

  virtual int columnCount(const QModelIndex &parent) const;

  virtual QVariant data(const QModelIndex &index, int role) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};


#endif // __INA_APP_MODELS_TABLEWRAPPER_HH__
