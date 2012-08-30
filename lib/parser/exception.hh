#ifndef __FLUC_UTILS_EXCEPTION_HH__
#define __FLUC_UTILS_EXCEPTION_HH__

#include <exception.hh>


namespace iNA {
namespace Parser {


/**
 * Base class for all parser exceptions.
 *
 * @ingroup parser
 */
class ParserError: public Exception
{
protected:
  /** Holds the line, where the error occures. */
  int _line;

public:
  /** Constructor. */
  ParserError(int line=-1);

  /** Constructor with message. */
  ParserError(const std::string &message, int line=-1);

  /** Copy constructor. */
  ParserError(const ParserError &other);

  /** Destructor. */
  virtual ~ParserError() throw();

  /** Returns the line number of the error. */
  int line() const;
};


/**
 * The lexer error class.
 *
 * @ingroup utils
 */
class LexerError: public ParserError
{
public:
  /**
   * Constructor.
   */
  LexerError(int line=-1);

  /**
   * Constructor with message.
   */
  LexerError(const std::string &message, int line=-1);

  /**
   * Copy constructor.
   */
  LexerError(const LexerError &other);

  /**
   * Destructor.
   */
  virtual ~LexerError() throw();
};


/**
 * The syntax/parser error class.
 *
 * @ingroup utils
 */
class SyntaxError: public ParserError
{
public:
  /**
   * Constructor.
   */
  SyntaxError(int line=-1);

  /**
   * Constructor with message.
   */
  SyntaxError(const std::string &message, int line=-1);

  /**
   * Copy constructor.
   */
  SyntaxError(const SyntaxError &other);

  /**
   * Destructor.
   */
  virtual ~SyntaxError() throw();
};


}
}
#endif // EXCEPTION_HH
