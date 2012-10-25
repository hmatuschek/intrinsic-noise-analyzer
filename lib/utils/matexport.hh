#ifndef __INA_UTILS_MATEXPORT_HH__
#define __INA_UTILS_MATEXPORT_HH__

#include <string>
#include <list>
#include <stdint.h>
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
  virtual ~MatFileElement() {};
  /** Returns the data size of the element. This does not include the header (tag) or padding
   * bytes. */
  virtual size_t dataSize() const = 0;
  /** Returns the storage size of the element, including header and optional padding bytes. */
  virtual size_t storageSize() const;
  /** Serializes the element into the given stream. The default implementation just serializes
   * the data block header. */
  virtual void serialize(std::ostream &stream) const;

protected:
  /** Holds the element type. */
  ElementType _type;
};


/**
 * A MAT file element holding a data block of a certain value.
 */
class MatFileValue : public MatFileElement
{
protected:
  /** Hidden constructor. */
  MatFileValue(MatFileElement::ElementType type);

public:
  /** Constructs a simple single value element. */
  MatFileValue(int64_t value);
  /** Constructs a simple single value element. */
  MatFileValue(uint64_t value);
  /** Constructs a simple single value element. */
  MatFileValue(double value);
  /** Constructs a simple UTF8 string value. */
  MatFileValue(const std::string &value);

  /** Destructor. */
  virtual  ~MatFileValue();
  /** Returns the data size of the element. */
  virtual size_t dataSize() const;
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
  /** Allocates an empty int8_t data block. */
  static MatFileValue *allocInt8(size_t num);
  /** Allocates an empty int32_t data block. */
  static MatFileValue *allocInt32(size_t num);
  /** Allocates an empty uint32_t data block. */
  static MatFileValue *allocUInt32(size_t num);
  /** Allocates an empty int64_t data block. */
  static MatFileValue *allocInt64(size_t num);
  /** Allocates an empty uint64_t data block. */
  static MatFileValue *allocUInt64(size_t num);
  /** Allocates an empty double data block. */
  static MatFileValue *allocDouble(size_t num);
  /** Allocates an empty UTF8 (char) data block. */
  static MatFileValue *allocUTF8(size_t num);

private:
  /** The union of values. */
  union {
    int8_t *asInt8;     ///< The data pointer as int8_t.
    int32_t *asInt32;   ///< The data pointer as int32_t.
    uint32_t *asUInt32; ///< The data pointer as uint32_t.
    int64_t *asInt64;   ///< The data pointer as int64_t.
    uint64_t *asUInt64; ///< The data pointer as uint64_t.
    double *asDouble;   ///< The data pointer as double.
    char *asString;     ///< The data pointer as char (UTF8).
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
    mxStructClass = 2,    ///< A structure.
    mxDoubleClass = 6,    ///< A double matrix.
    mxInt64Class  = 14,   ///< An integer matrix of int64_t.
    mxUInt64Class = 15    ///< An integer matrix of uint64_t.
  } ArrayType;

protected:
  /** Constructor. */
  MatFileComplexElement(ArrayType type);
  /** Destructor. */
  virtual ~MatFileComplexElement();
  /** Adds a sub element to the complex type. */
  void add(MatFileElement *element);

public:
  /** Returns the data block size. */
  size_t dataSize() const;
  /** Serializes the complex type into the given stream. */
  void serialize(std::ostream &stream) const;

protected:
  /** Holds the type of the complex element. */
  ArrayType _arrayType;
  /** Holds the sub elements of the complex element. */
  std::list<MatFileElement *> _subelements;
};


/** Implements a martrix (Eigen double matrix) as a MAT file elemement.
 * Each matrix element consists of the matrix itself and a name attribute, used
 * to address the matrix in the MAT file. Note that the name must be a valid Matlab identifier, not
 * all ASCII chars are allowed for that name. iNA does not check if the name has the correct format.
 */
class MatFileMatrixElement : public MatFileComplexElement
{
public:
  /** Constructor. */
  MatFileMatrixElement(const std::string &name, const Eigen::MatrixXd &values);
};


/**
 * This class implements the export of Eigen matices as Matlab 5 data files (MAT). These
 * files can be loaded by Matlab, Octave and Python (with scipy).
 *
 * @code
 * Eigen::MatrixXd X(4,4);
 * // Add some values to X
 *
 * // Create MAT file object and add data.
 * MatFile mat;
 * mat.add("X", X);
 *
 * // open file for writing:
 * fstream output("PATH/TO/FILE"); output.open();
 * // Serialize data into file:
 * mat.serialize(output);
 * // close file.
 * output.close();
 * @endcode
 *
 * @todo Find out how to export a vector of UTF8 strings in MAT files. This is needed to properly
 *       export table headers into mat files.
 *
 * @todo Find out how to export R RDA files. This implements tables and therefore allows to properly
 *       map tables generated by iNA to RDA files.
 *
 * @ingroup utils
 */
class MatFile
{
public:
  /** Empty constructor. */
  MatFile();

  /** Destructor. Also frees all all elements of the file.*/
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
