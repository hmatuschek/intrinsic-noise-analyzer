#ifndef __FLUC_EXCEPTION_HH__
#define __FLUC_EXCEPTION_HH__

#include <exception>
#include <sstream>
#include <ginac/ginac.h>


namespace Fluc
{

/**
 * Base class for all exceptions.
 *
 * This class is derived from @c std::exception. Therefore an uncatched exception will be printed
 * by default.
 *
 * This class is also derived from std::ostringstream, this allows to append text to the message
 * of the exception once an instance is created.
 */
class Exception: public std::exception
{
protected:
  /**
   * Buffer for the message.
   */
  std::string message;

protected:
  /**
   * Constructs a base exception without a message.
   *
   * The constructor is hidden to avoid direct instantiation, please use one of the more
   * precise exceptions below.
   */
  Exception();

  /**
   * Constructs a base exception with the given message
   *
   * The constructor is hidden to avoid direct instantiation, please use one of the more
   * precise exceptions below.
   */
  Exception(const std::string &message);

public:
  /**
   * Copy constructor.
   */
  Exception(const Exception &other);

  /**
   * Is needed by the std::exception interface. Does nothing.
   */
  virtual ~Exception() throw ();

  /**
   * Returns the message of the exception. This method is needed by the std::exception interface.
   */
  virtual const char *what() const throw();

  Exception &operator<< (unsigned char c);
  Exception &operator<< (const char *text);
  Exception &operator<< (const std::string &text);
  Exception &operator<< (size_t value);
  Exception &operator<< (unsigned int value);
  Exception &operator<< (double value);
  Exception &operator<< (const GiNaC::ex &expression);
};



/**
 * Base class for all compiler related exceptions.
 */
class CompilerException : public Exception
{
public:
  /**
   * Constructs a compiler exception with no message.
   */
  CompilerException();

  /**
   * Constructs a compiler exception with the given message.
   */
  CompilerException(const std::string &message);

  /**
   * Copy constructor.
   */
  CompilerException(const CompilerException &other);

  virtual ~CompilerException() throw();
};



/**
 * This exception will be thrown if a symbol/name/identifier can not be resolved.
 */
class SymbolError : public CompilerException
{
public:
  /**
   * Constructs a symbol exception with no message.
   */
  SymbolError();

  /**
   * Constructs a symbol exception with the given message.
   */
  SymbolError(const std::string &message);

  /**
   * Copy constructor.
   */
  SymbolError(const SymbolError &other);

  virtual ~SymbolError() throw();
};



/**
 * General exception to signal a semantic error.
 */
class SemanticError : public CompilerException
{
public:
  /**
   * Constructs a semantic exception with no message.
   */
  SemanticError();

  /**
   * Constructs a semantic exception with the given message.
   */
  SemanticError(const std::string &message);

  /**
   * Copy constructor.
   */
  SemanticError(const SemanticError &other);

  virtual ~SemanticError() throw();
};



/**
 * General exception to signal a type-error.
 */
class TypeError : public CompilerException
{
public:
  /**
   * Constructs a type exception with no message.
   */
  TypeError();

  /**
   * Constructs a type exception with the given message.
   */
  TypeError(const std::string &message);

  /**
   * Copy constructor.
   */
  TypeError(const TypeError &other);

  virtual ~TypeError() throw();
};



/**
 * General exception to signal an internal error. This kind of error are for example missing
 * implementation of features or signaling a bug in the compiler code.
 */
class InternalError : public CompilerException
{
public:
  /**
   * Constructs an internal exception with no message.
   */
  InternalError();

  /**
   * Constructs an internal exception with the given message.
   */
  InternalError(const std::string &message);

  /**
   * Copy constructor.
   */
  InternalError(const InternalError &other);

  virtual ~InternalError() throw();
};



/**
 * Wraps libsbml errors into an exception.
 */
class SBMLParserError: public Exception
{
public:
  /**
   * Constructs an empty exception message.
   */
  SBMLParserError();

  /**
   * Constructs an exception with given message.
   */
  SBMLParserError(const std::string &message);

  /**
   * Copyconstructor.
   */
  SBMLParserError(const SBMLParserError &other);

  /**
   * Destructor.
   */
  virtual ~SBMLParserError() throw();
};



/**
 * This exception will be thrown if the Ast::Assembler instance finds a SBML element, that is
 * not supported by this software.
 */
class SBMLFeatureNotSupported : public SBMLParserError
{
public:
  /**
   * Constructs a SBMLFeatureNotSupported exception without message.
   */
  SBMLFeatureNotSupported();

  /**
   * Constructs exception with given message.
   */
  SBMLFeatureNotSupported(const std::string &message);

  /**
   * Copy constructor.
   */
  SBMLFeatureNotSupported(const SBMLFeatureNotSupported &other);

  /**
   * Destructor.
   */
  virtual ~SBMLFeatureNotSupported() throw ();
};



/**
 * Will be thrown if there is an error during some calculations.
 */
class RuntimeError : public Exception
{
public:
  /**
   * Default constructor, without any message.
   */
  RuntimeError();

  /**
   * Constructs a runtime-exception with the given message.
   */
  RuntimeError(const std::string &message);

  /**
   * Copy constructor.
   */
  RuntimeError(const RuntimeError &ohter);

  /**
   * Destructor.
   */
  virtual ~RuntimeError() throw ();
};



/**
 * Will be thrown if some numerical error occures during any calculation.
 */
class NumericError : public RuntimeError
{
public:
  /**
   * Default constructor, w/o any message.
   */
  NumericError();

  /**
   * Constructs a numeric error with the given message.
   */
  NumericError(const std::string &message);

  /**
   * Copy constructor.
   */
  NumericError(const NumericError &other);

  /**
   * Destructor.
   */
  virtual ~NumericError() throw ();
};

}

#endif
