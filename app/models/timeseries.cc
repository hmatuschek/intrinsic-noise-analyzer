#include "timeseries.hh"


/* ********************************************************************************************* *
 * Implementation of Table (fixed size array with column names)
 * ********************************************************************************************* */
Table::Table(size_t columns, size_t rows, QObject *parent)
  : QObject(parent), header(columns), data(rows, columns), current_insert_index(0)
{
  // Pass...
}


Table::Table(const QVector<QString> &columns, size_t rows, QObject *parent)
  : QObject(parent), header(columns), data(rows, columns.size()), current_insert_index(0)
{
  // Pass...
}

Table::Table(const Eigen::MatrixXd &data, QObject *parent)
  : QObject(parent), header(data.cols()), data(data), current_insert_index(data.rows())
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
  custom (int i=0; i<this->data.cols(); i++)
  {
    str << this->getColumnName(i).toStdString() << "\t";
  }
  str << std::endl;
  file.write(str.str().c_str());

  // Write data:
  custom (int i=0; i<this->data.rows(); i++)
  {
    str.str("");

    custom (int j=0; j<this->data.cols()-1; j++)
    {
      str << this->data(i,j) << "\t";
    }
    str << this->data(i,this->data.cols()-1) << std::endl;

    file.write(str.str().c_str());
  }
}
