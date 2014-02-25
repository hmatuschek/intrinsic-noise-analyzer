#include "timeseries.hh"


/* ********************************************************************************************* *
 * Implementation of Table (fixed size array with column names)
 * ********************************************************************************************* */
Table::Table(size_t columns, size_t rows, QObject *parent)
  : QObject(parent), _header(columns), _data(rows, columns), _current_insert_index(0)
{
  // Pass...
}


Table::Table(const QVector<QString> &columns, size_t rows, QObject *parent)
  : QObject(parent), _header(columns), _data(rows, columns.size()), _current_insert_index(0)
{
  // Pass...
}

Table::Table(const Eigen::MatrixXd &data, QObject *parent)
  : QObject(parent), _header(data.cols()), _data(data), _current_insert_index(data.rows())
{
  // Pass...
}

Table::Table(const Table &other)
  : QObject(), _header(other._header), _data(other._data),
    _current_insert_index(other._current_insert_index)
{
  // pass...
}


void
Table::resize(size_t columns,size_t rows) {
  _header.resize(columns);
  matrix().resize(rows,columns);
}

size_t
Table::getNumColumns() const {
  return this->_header.size();
}


size_t
Table::getNumRows() const {
  return this->_data.rows();
}


void
Table::setColumnName(size_t idx, const QString &name) {
  this->_header[idx] = name;
}


const QString &
Table::getColumnName(size_t idx) const {
  return this->_header[idx];
}


void
Table::append(Eigen::VectorXd &values) {
  this->_data.row(this->_current_insert_index) = values;
  this->_current_insert_index++;
}


const double &
Table::operator ()(size_t i, size_t j) const {
  return this->_data(i,j);
}

double &
Table::operator ()(size_t i, size_t j) {
  return this->_data(i,j);
}

Eigen::MatrixXd &
Table::matrix() {
  return this->_data;
}

Eigen::VectorXd
Table::getRow(size_t i) {
  return this->_data.row(i);
}

Eigen::VectorXd
Table::getColumn(size_t i) {
  return this->_data.col(i);
}


void
Table::saveAsText(QFile &file) {
  if (!file.isOpen() || !file.isWritable() || 0 == this->_data.cols()) {
    return;
  }

  std::stringstream str;

  // Write header:
  str << "# ";
  for (int i=0; i<this->_data.cols(); i++) {
    str << this->getColumnName(i).toStdString() << "\t";
  }
  str << std::endl;
  file.write(str.str().c_str());

  // Write data:
  for (int i=0; i<this->_data.rows(); i++) {
    str.str("");

    for (int j=0; j<this->_data.cols()-1; j++) {
      str << this->_data(i,j) << "\t";
    }
    str << this->_data(i,this->_data.cols()-1) << std::endl;

    file.write(str.str().c_str());
  }
}
