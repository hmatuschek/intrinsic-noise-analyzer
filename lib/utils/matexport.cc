#include "matexport.hh"
#include <config.hh>
#include <ostream>

using namespace iNA;
using namespace iNA::Utils;



/* ******************************************************************************************** *
 * Implementation of the MAT file interface.
 * ******************************************************************************************** */
MatFile::MatFile()
  : _elements()
{
  // pass...
}

MatFile::~MatFile()
{
  for (std::list<MatFileElement *>::iterator element = _elements.begin(); element != _elements.end(); element++)
  {
    delete *element;
  }
}


void MatFile::add(int64_t value) {
  _elements.push_back(new MatFileValue(value));
}

void MatFile::add(uint64_t value) {
  _elements.push_back(new MatFileValue(value));
}

void MatFile::add(double value) {
  _elements.push_back(new MatFileValue(value));
}

void MatFile::add(const std::string &value) {
  _elements.push_back(new MatFileValue(value));
}

void MatFile::add(const std::string &name, const Eigen::MatrixXd &value) {
  _elements.push_back(new MatFileMatrixElement(name, value));
}

void
MatFile::serialize(std::ostream &stream)
{
  // Serialize header text (124 bytes):
  std::string header_text = "Matlab 5.0 MAT-file, created with iNA " INA_VERSION_STRING;
  if (124 < header_text.size()) { header_text = header_text.substr(0, 124); }
  stream << header_text;
  for (size_t i=header_text.size(); i<124; i++) { stream.put(0x00); }
  // serialize header version:
  stream.put(0x01); stream.put(0x00);
  // serialize endian indicator:
  short endian = short(('M'<<8)||'I');
  stream.write((char *)(&endian), 2);

  // Serialize elements:
  for (std::list<MatFileElement *>::iterator element = _elements.begin();
       element != _elements.end(); element++)
  {
    // Serialize element:
    (*element)->serialize(stream);
  }
}




/* ******************************************************************************************** *
 * Implementation of the general MAT file element
 * ******************************************************************************************** */
MatFileElement::MatFileElement(ElementType type)
  : _type(type)
{
  // Pass...
}

size_t
MatFileElement::storageSize() const {
  size_t s = this->dataSize()+8;
  s += ((8 - (s%8))%8);
  return s;
}

void
MatFileElement::serialize(std::ostream &stream) const
{
  uint32_t type_field = uint32_t(_type);
  uint32_t length_field = uint32_t(dataSize());
  stream.write((char *)(&type_field), sizeof(uint32_t));
  stream.write((char *)(&length_field), sizeof(uint32_t));
}



/* ******************************************************************************************** *
 * Implementation of the MAT file element for values
 * ******************************************************************************************** */
MatFileValue::MatFileValue(MatFileElement::ElementType type)
  : MatFileElement(type)
{
  // pass...
}

MatFileValue::MatFileValue(int64_t value)
  : MatFileElement(miInt64)
{
  _value.asInt64 = new int64_t[1];
  _value.asInt64[0] = value;
  _num_elements = 1;
}

MatFileValue::MatFileValue(uint64_t value)
  : MatFileElement(miUInt64)
{
  _value.asUInt64 = new uint64_t[1];
  _value.asUInt64[0] = value;
  _num_elements = 1;
}

MatFileValue::MatFileValue(double value)
  : MatFileElement(miInt64)
{
  _value.asDouble = new double[1];
  _value.asDouble[0] = value;
  _num_elements = 1;
}

MatFileValue::MatFileValue(const std::string &value)
  : MatFileElement(miUTF8)
{
  _value.asString = new char[value.size()];
  memcpy(_value.asString, value.c_str(), value.size());
  _num_elements = value.size();
}

MatFileValue::~MatFileValue()
{
  switch (_type) {
  case miInt8:   delete _value.asInt8; break;
  case miInt32:  delete _value.asInt32; break;
  case miUInt32: delete _value.asUInt32; break;
  case miInt64:  delete _value.asInt64; break;
  case miUInt64: delete _value.asUInt64; break;
  case miDouble: delete _value.asDouble; break;
  case miUTF8:   delete _value.asString; break;
  default: break;
  }
}


size_t
MatFileValue::dataSize() const
{
  switch (_type) {
  case miInt64:  return _num_elements*sizeof(long);
  case miUInt64: return _num_elements*sizeof(unsigned long);
  case miDouble: return _num_elements*sizeof(double);
  case miUTF8:   return _num_elements*sizeof(char);
  default: break;
  }

  return 0;
}

void
MatFileValue::serialize(std::ostream &stream) const
{
  // Serialize header!
  MatFileElement::serialize(stream);

  // Serialize data
  switch (_type) {
  case miInt64:
    stream.write((char *)(_value.asInt64), _num_elements*sizeof(long));
    break;
  case miUInt64:
    stream.write((char *)(_value.asUInt64), _num_elements*sizeof(unsigned long));
    break;
  case miDouble:
    stream.write((char *)(_value.asDouble), _num_elements*sizeof(double));
    break;
  case miUTF8:
    stream.write((char *)(_value.asString), _num_elements*sizeof(char));
    break;
  default:
    break;
  }

  // Add padding bytes to multiple of 64bit
  size_t padd = (8 - dataSize()) % 8;
  for (size_t i=0; i<padd; i++) { stream.put(0x00); }
}


int8_t  *MatFileValue::dataInt8() { return _value.asInt8; }
int32_t *MatFileValue::dataInt32() { return _value.asInt32; }
uint32_t *MatFileValue::dataUInt32() { return _value.asUInt32; }
int64_t *MatFileValue::dataInt64() { return _value.asInt64; }
uint64_t *MatFileValue::dataUInt64() { return _value.asUInt64; }
double *MatFileValue::dataDouble() { return _value.asDouble; }
char *MatFileValue::dataUTF8() { return _value.asString; }

MatFileValue *
MatFileValue::allocInt8(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miInt8);
  element->_value.asInt8 = new int8_t[num];
  return element;
}

MatFileValue *
MatFileValue::allocInt32(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miInt32);
  element->_value.asInt32 = new int32_t[num];
  return element;
}

MatFileValue *
MatFileValue::allocUInt32(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miUInt32);
  element->_value.asUInt32 = new uint32_t[num];
  return element;
}

MatFileValue *
MatFileValue::allocInt64(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miInt64);
  element->_value.asInt64 = new int64_t[num];
  return element;
}

MatFileValue *
MatFileValue::allocUInt64(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miUInt64);
  element->_value.asUInt64 = new uint64_t[num];
  return element;
}

MatFileValue *
MatFileValue::allocDouble(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miDouble);
  element->_value.asDouble = new double[num];
  return element;
}

MatFileValue *
MatFileValue::allocUTF8(size_t num)
{
  MatFileValue *element = new MatFileValue(MatFileElement::miUTF8);
  element->_value.asString = new char[num];
  return element;
}



/* ******************************************************************************************** *
 * Implementation of generic MAT file matrix element
 * ******************************************************************************************** */
MatFileComplexElement::MatFileComplexElement(ArrayType type)
  : MatFileElement(miMatrix), _arrayType(type)
{
  // pass...
}

MatFileComplexElement::~MatFileComplexElement()
{
  for (std::list<MatFileElement*>::iterator item=_subelements.begin(); item!=_subelements.end(); item++) {
    delete *item;
  }
}

void
MatFileComplexElement::add(MatFileElement *element) {
  _subelements.push_back(element);
}

size_t
MatFileComplexElement::dataSize() const {
  // Data size is sum of storafge sizes of all sub elements.
  size_t s=0;
  for (std::list<MatFileElement *>::const_iterator item=_subelements.begin(); item!=_subelements.end(); item++) {
    s += (*item)->storageSize();
  }
  return s;
}

void
MatFileComplexElement::serialize(std::ostream &stream) const
{
  // Serialize generic header:
  MatFileElement::serialize(stream);

  // now serialize all sub-elements:
  for (std::list<MatFileElement *>::const_iterator item=_subelements.begin(); item!=_subelements.end(); item++) {
    (*item)->serialize(stream);
  }
}



/* ******************************************************************************************** *
 * Implementation of double matrix element:
 * ******************************************************************************************** */
MatFileMatrixElement::MatFileMatrixElement(const std::string &name, const Eigen::MatrixXd &values)
  : MatFileComplexElement(mxDoubleClass)
{
  // Allocate sub elements:
  MatFileValue *array_flags = MatFileValue::allocUInt32(2);
  MatFileValue *array_dimensions = MatFileValue::allocInt32(2);
  MatFileValue *array_name = MatFileValue::allocInt8(name.size());
  MatFileValue *array_data = MatFileValue::allocDouble(values.rows()*values.cols());

  // Add sub elements:
  add(array_flags); add(array_dimensions); add(array_name); add(array_data);

  // Assemble array flags:
  array_flags->dataUInt32()[0] = 0;
  array_flags->dataUInt32()[1] = (0x0400u | uint8_t(_arrayType));

  // Assemble dimensions array:
  array_dimensions->dataInt32()[0] = values.rows();
  array_dimensions->dataInt32()[1] = values.cols();

  // Copy name:
  memcpy(array_name->dataUTF8(), name.c_str(), name.size());

  // Copy data (column major / Fortran order):
  size_t idx=0;
  for (int j=0; j<values.cols(); j++) {
    for (int i=0; i<values.rows(); i++, idx++) {
      array_data->dataDouble()[idx] = values(i,j);
    }
  }
}
