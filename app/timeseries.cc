#include "timeseries.hh"


/* ********************************************************************************************* *
 * Implementation of DataConsumer, base interface for all data handlers with append()
 * ********************************************************************************************* */
DataConsumer::DataConsumer(QObject *parent)
  : QObject(parent)
{
  // Pass...
}



/* ********************************************************************************************* *
 * Implementation of TimeSeries, a streaming class for Eigen::Vectors.
 * ********************************************************************************************* */
TimeSeries::TimeSeries(size_t columns, QObject *parent) :
  DataConsumer(parent), columns(columns)
{
  // Pass...
}


TimeSeries::TimeSeries(const QVector<QString> &columns, QObject *parent)
  : DataConsumer(parent), columns(columns)
{
  // Pass...
}


size_t
TimeSeries::getNumColumns() const
{
  return this->columns.size();
}


void
TimeSeries::setColumnName(size_t idx, const QString &name)
{
  this->columns[idx] = name;
}


const QString &
TimeSeries::getColumnName(size_t idx) const
{
  return this->columns[idx];
}

DataQueue *
TimeSeries::getQueue()
{
  DataQueue *queue = new DataQueue(this);
  this->queues.append(queue);

  return queue;
}


void
TimeSeries::append(Eigen::VectorXd &values)
{
  for (QList<DataQueue *>::iterator iter = this->queues.begin();
       iter != this->queues.end(); iter++)
  {
    (*iter)->pushData(values);
  }
}




/* ********************************************************************************************* *
 * Implementation of the data queue:
 * ********************************************************************************************* */
DataQueue::DataQueue(QObject *parent)
  : QObject(parent), mutex(), queue()
{
  // Pass...
}


bool
DataQueue::hasData()
{
  QMutexLocker locker(&(this->mutex));

  return 0 != this->queue.size();
}


void
DataQueue::pushData(Eigen::VectorXd &data)
{
  QMutexLocker locker(&(this->mutex));

  this->queue.push_front(data);

  if (1 == this->queue.size())
  {
    emit this->dataAvailable();
  }

  locker.unlock();
}


const Eigen::VectorXd &
DataQueue::getData() const
{
  QMutexLocker locker(&(this->mutex));

  return this->queue.back();
}


void
DataQueue::popData()
{
  QMutexLocker locker(&(this->mutex));

  this->queue.pop_back();
}



/* ********************************************************************************************* *
 * Implementation of Table (fixed size array with column names)
 * ********************************************************************************************* */
Table::Table(size_t columns, size_t rows, QObject *parent)
  : DataConsumer(parent), header(columns), data(rows, columns), current_insert_index(0)
{
  // Pass...
}


Table::Table(const QVector<QString> &columns, size_t rows, QObject *parent)
  : DataConsumer(parent), header(columns), data(rows, columns.size()), current_insert_index(0)
{
  // Pass...
}

Table::Table(const Eigen::MatrixXd &data, QObject *parent)
  : DataConsumer(parent), header(data.cols()), data(data), current_insert_index(data.rows())
{
  // Pass...
}

void
Table::resize(size_t columns,size_t rows)
{
  header.resize(columns);
  matrix().resize(rows,columns);
}

size_t
Table::getNumColumns() const
{
  return this->header.size();
}


size_t
Table::getNumRows() const
{
  return this->data.rows();
}


void
Table::setColumnName(size_t idx, const QString &name)
{
  this->header[idx] = name;
}


const QString &
Table::getColumnName(size_t idx) const
{
  return this->header[idx];
}


void
Table::append(Eigen::VectorXd &values)
{
  this->data.row(this->current_insert_index) = values;
  this->current_insert_index++;
}


const double &
Table::operator ()(size_t i, size_t j) const
{
  return this->data(i,j);
}

double &
Table::operator ()(size_t i, size_t j)
{
  return this->data(i,j);
}

Eigen::MatrixXd &
Table::matrix()
{
  return this->data;
}

Eigen::VectorXd
Table::getRow(size_t i)
{
  return this->data.row(i);
}

Eigen::VectorXd
Table::getColumn(size_t i)
{
  return this->data.col(i);
}


void
Table::saveAsText(QFile &file)
{
  if (!file.isOpen() || !file.isWritable() || 0 == this->data.cols())
  {
    return;
  }

  std::stringstream str;

  // Write header:
  str << "# ";
  for (int i=0; i<this->data.cols(); i++)
  {
    str << this->getColumnName(i).toStdString() << "\t";
  }
  str << std::endl;
  file.write(str.str().c_str());

  // Write data:
  for (int i=0; i<this->data.rows(); i++)
  {
    str.str("");

    for (int j=0; j<this->data.cols()-1; j++)
    {
      str << this->data(i,j) << "\t";
    }
    str << this->data(i,this->data.cols()-1) << std::endl;

    file.write(str.str().c_str());
  }
}
