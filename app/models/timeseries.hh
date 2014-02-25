#ifndef TIMESERIES_HH
#define TIMESERIES_HH

#include <QObject>
#include <QVector>
#include <QList>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>

#include <ginacsupportforeigen.hh>

/** Represents a table of data (ie. a time-series) of some covariates with column names.
 * @ingroup gui */
class Table : public QObject
{
  Q_OBJECT

public:
  /** Constructs an empty (uninitialized) data-table with given row- and column-count. */
  explicit Table(size_t columns, size_t rows, QObject *parent=0);
  /** Constructs an empty (uninitilized) data-table with given column-labels and row-count. */
  explicit Table(const QVector<QString> &columns, size_t rows, QObject *parent=0);
  /** Constructs a data-table from given matrix. */
  explicit Table(const Eigen::MatrixXd &_data, QObject *parent=0);
  /** Copy constructor. */
  Table(const Table &other);

  /** Resizes the table. */
  void resize(size_t columns, size_t rows);
  /** Returns the number of columns. */
  virtual size_t getNumColumns() const;
  /** Returns the number of rows. */
  size_t getNumRows() const;

  /** (Re-) Sets the column label. */
  virtual void setColumnName(size_t idx, const QString &name);
  /** Retunrs the column-label. */
  virtual const QString &getColumnName(size_t idx) const;

  /** Appends some data. */
  virtual void append(Eigen::VectorXd &values);

  /** Returns the value at the @c i -th row and @c j -th column. */
  const double &operator() (size_t i, size_t j) const;
  /** Returns the value at the @c i -th row and @c j -th column. */
  double &operator() (size_t i, size_t j);

  /** Saves the table as text-file. */
  void saveAsText(QFile &file);

  /** Returns the matrix, holding the data. */
  Eigen::MatrixXd &matrix();
  /** Returns a certain row as a @c Eigen::VectorXd. */
  Eigen::VectorXd getRow(size_t i);
  /** Returns a certain column as a @c Eigen::VectorXd. */
  Eigen::VectorXd getColumn(size_t i);


protected:
  /** Holds the column-names. */
  QVector<QString> _header;
  /** Holds the actual data. */
  Eigen::MatrixXd _data;
  /** Index of the next, empty row in the table. */
  size_t _current_insert_index;
};


#endif // TIMESERIES_HH
