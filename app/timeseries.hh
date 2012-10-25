#ifndef TIMESERIES_HH
#define TIMESERIES_HH

#include <QObject>
#include <QVector>
#include <QList>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>

#include <eigen3/Eigen/Eigen>



/**
 * Thread save Queue implementation.
 *
 * @deprecated Only the @c Table model is used to store data.
 *
 * @ingroup gui
 */
class DataQueue : public QObject
{
  Q_OBJECT

protected:
  /**
   * Holds the mutex to make the object thread-save.
   */
  mutable QMutex mutex;

  /**
   * Holds the queue of rows.
   */
  QList<Eigen::VectorXd> queue;


public:
  /**
   * Constructor.
   */
  explicit DataQueue(QObject *parent=0);

  /**
   * Returns true, if there is some data in the queue.
   */
  bool hasData();

  /**
   * Appends a data-row to the queue.
   */
  void pushData(Eigen::VectorXd &data);

  /**
   * Returns a weak reference to the first data-row in the queue.
   */
  const Eigen::VectorXd &getData() const;

  /**
   * Removes the first data-row from the queue.
   */
  void popData();


signals:
  /**
   * Will be emitted if the queue gets non-empty.
   */
  void dataAvailable();
};


/**
 * Abstract base class for object to receive some data from a @c DataQueue.
 *
 * @deprecated See @c DataQueue
 *
 * @ingroup gui
 */
class DataConsumer : public QObject
{
  Q_OBJECT

public:
  /**
   * Constructor.
   */
  explicit DataConsumer(QObject *parent=0);

  /**
   * Returns the number of columns received.
   */
  virtual size_t getNumColumns() const = 0;

  /**
   * (Re-) Sets the column name.
   */
  virtual void setColumnName(size_t idx, const QString &name) = 0;

  /**
   * Returns the column-name.
   */
  virtual const QString &getColumnName(size_t idx) const = 0;

  /**
   * Appends a row.
   */
  virtual void append(Eigen::VectorXd &values) = 0;
};



/**
 * A simple data-consumer to receive some data from a @c DataQueue.
 *
 * This class implements a @c DataConsumer interface and provides @c DataQueue to forward data.
 *
 * @deprecated @see @c DataQueue
 *
 * @ingroup gui
 */
class TimeSeries : public DataConsumer
{
  Q_OBJECT

protected:
  /**
   * Vector of column-names.
   */
  QVector<QString> columns;

  /**
   * Holds the list of data-queues to forward data.
   */
  QList<DataQueue *> queues;


public:
  /**
   * Constructor with unnamed columns.
   */
  explicit TimeSeries(size_t columns, QObject *parent = 0);

  /**
   * Constructor with column-names.
   */
  explicit TimeSeries(const QVector<QString> &columns, QObject *parent=0);

  /**
   * Returns the number of columns.
   **/
  virtual size_t getNumColumns() const;

  /**
   * (Re-) Sets the column-name.
   */
  virtual void setColumnName(size_t idx, const QString &name);

  /**
   * Returns the column-name.
   */
  virtual const QString &getColumnName(size_t idx) const;

  /**
   * Appends some data to the time-series.
   */
  virtual void append(Eigen::VectorXd &values);

  /**
   * Returns a certain row of the time-series.
   */
  Eigen::VectorXd getRow(size_t i);

  /**
   * Returns a certain column of the time-series.
   */
  Eigen::VectorXd getColumn(size_t i);

  /**
   * Returns a new queue, connected to the time-series.
   */
  DataQueue *getQueue();
};



/**
 * Represents a table of data (ie. a time-series) of some covariates.
 *
 * @todo Fix @c Table constructor to always take column-labels!
 * @todo Fix @c Table constructor to always take column-units!
 * @todo Drop data-consumer interface.
 *
 * @ingroup gui
 */
class Table : public DataConsumer
{
  Q_OBJECT

protected:
  /**
   * Holds the column-names.
   */
  QVector<QString> header;

  /**
   * Holds the actual data.
   */
  Eigen::MatrixXd data;

  /**
   * Index of the next, empty row in the table.
   */
  size_t current_insert_index;


public:
  /**
   * Constructs an empty (uninitialized) data-table with given row- and column-count.
   * @bug Clean up that shit here!
   */
  explicit Table(size_t columns, size_t rows, QObject *parent=0);

  /**
   * Constructs an empty (uninitilized) data-table with given column-labels and row-count.
   * @bug Clean up that shit here!
   */
  explicit Table(const QVector<QString> &columns, size_t rows, QObject *parent=0);

  /**
   * Constructs a data-table from given matrix.
   */
  explicit Table(const Eigen::MatrixXd &data, QObject *parent=0);


  void resize(size_t columns, size_t rows);

  /**
   * Returns the number of columns.
   */
  virtual size_t getNumColumns() const;

  /**
   * Returns the number of rows.
   */
  size_t getNumRows() const;

  /**
   * (Re-) Sets the column label.
   */
  virtual void setColumnName(size_t idx, const QString &name);

  /**
   * Retunrs the column-label.
   */
  virtual const QString &getColumnName(size_t idx) const;

  /**
   * Appends some data.
   */
  virtual void append(Eigen::VectorXd &values);

  /**
   * Returns the value at the @c i -th row and @c j -th column.
   */
  const double &operator() (size_t i, size_t j) const;
  double &operator() (size_t i, size_t j);
  /**
   * Saves the table as text-file.
   */
  void saveAsText(QFile &file);

  /**
   * Returns the matrix, holding the data.
   */
  Eigen::MatrixXd &matrix();

  /**
   * Returns a certain row as a @c Eigen::VectorXd.
   */
  Eigen::VectorXd getRow(size_t i);

  /**
   * Returns a certain column as a @c Eigen::VectorXd.
   */
  Eigen::VectorXd getColumn(size_t i);
};


#endif // TIMESERIES_HH
