#ifndef __INA_UTILS_MATEXPORT_HH__
#define __INA_UTILS_MATEXPORT_HH__

#include <string>
#include <list>
#include <eigen3/Eigen/Eigen>


namespace iNA {
namespace Utils {


/**
 * Abstract call defining the interface for all MAT file elements.
 * This class is used internal and may not be used directly by the application.
 */
class MatFileElement
{
public:
  /** Lists all element types. */
  typedef enum {
    miInt8   = 1,
    miInt32  = 5,
    miUInt32 = 6,
    miDouble = 9,
    miInt64  = 12,
    miUInt64 = 13,
    miMatrix = 14,
    miUTF8   = 16
  } ElementType;

protected:
  /** Hidden constructor. */
  MatFileElement(ElementType type);

public:
  /** Returns the size of the element. */
  virtual size_t getSize() const = 0;
  /** Serializes the element into the given stream. */
  virtual void serialize(std::ostream &stream) const;

protected:
  /** Holds the element type. */
  ElementType _type;
};


/**
 * A MAT file element holding a single value.
 */
class MatFileValue : public MatFileElement
{
protected:
  /** Hidden constructor. */
  MatFileValue(MatFileElement::ElementType type);

public:
  MatFileValue(int64_t value);
  MatFileValue(uint64_t value);
  MatFileValue(double value);
  MatFileValue(const std::string &value);

  /** Destructor. */
  virtual  ~MatFileValue();
  /** Returns the storage size of the element. */
  virtual size_t getSize() const;
  /** Serializes the value element into the given stream. */
  virtual void serialize(std::ostream &stream) const;

  /** Returns the data as an int8_t pointer. */
  int8_t *dataInt8();
  /** Returns the data as an int32_t pointer. */
  int32_t *dataInt32();
  /** Returns the data as an uint32_t pointer. */
  uint32_t *dataUInt32();
  /** Returns the data as an int64_t pointer. */
  int64_t *dataInt64();
  /** Returns the data as an uint64_t pointer. */
  uint64_t *dataUInt64();
  /** Returns the data as a double pointer. */
  double *dataDouble();
  /** Returns the data as string pointer. */
  char *dataUTF8();

public:
  static MatFileValue *allocInt8(size_t num);
  static MatFileValue *allocInt32(size_t num);
  static MatFileValue *allocUInt32(size_t num);
  static MatFileValue *allocInt64(size_t num);
  static MatFileValue *allocUInt64(size_t num);
  static MatFileValue *allocDouble(size_t num);
  static MatFileValue *allocUTF8(size_t num);

private:
  /** The union of values. */
  union {
    int8_t *asInt8;
    int32_t *asInt32;
    uint32_t *asUInt32;
    int64_t *asInt64;
    uint64_t *asUInt64;
    double *asDouble;
    char *asString;
  } _value;

  /** Holds the number of elements. */
  size_t _num_elements;
};


/**
 * Generic MAT file matrix element. Note that Matlab calls everything a "matrix" that
 * is not just a series of numbers or chars.
 */
class MatFileComplexElement : public MatFileElement
{
public:
  /** List of supported array class types. */
  typedef enum {
    mxStructClass = 2,
    mxDoubleClass = 6,
    mxInt64Class  = 14,
    mxUInt64Class = 15
  } ArrayType;

protected:
  MatFileComplexElement(ArrayType type);
  virtual ~MatFileComplexElement();

  void add(MatFileElement *element);

public:
  size_t getSize() const;
  void serialize(std::ostream &stream) const;

protected:
  ArrayType _arrayType;
  std::list<MatFileElement *> _subelements;
};


/** Implements a martrix (Eigen double matrix) as a MAT file elemement. */
class MatFileMatrixElement : public MatFileComplexElement
{
public:
  MatFileMatrixElement(const std::string &name, const Eigen::MatrixXd &values);
};


/**
 * This class implements the export of Eigen matices as Matlab 4 data files. These
 * files can be loaded by Matlab and Octave.
 *
 * @ingroup utils
 */
class MatFile
{
public:
  /** Empty constructor. */
  MatFile();

  /** Destructor. */
  ~MatFile();

  /** Serializes the MAT file into the given stram. */
  void serialize(std::ostream &stream);

  /** Adds a single signed integer value to the file. */
  void add(int64_t value);
  /** Adds a single unsigned integer value to the file. */
  void add(uint64_t value);
  /** Adds a single double floating point value to the file. */
  void add(double value);
  /** Adds a string to the file. */
  void add(const std::string &value);
  /** Adds a matrix of double values to the file. */
  void add(const std::string &name, const Eigen::MatrixXd &value);

private:
  /** Holds the elements of the MAT file. */
  std::list<MatFileElement *> _elements;
};


}
}

#endif // __INA_UTILS_MATEXPORT_HH__
