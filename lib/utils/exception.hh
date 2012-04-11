#ifndef __FLUC_UTILS_EXCEPTION_HH__
#define __FLUC_UTILS_EXCEPTION_HH__

#include <exception.hh>


namespace Fluc {
namespace Utils {


/**
 * Base class for all parser exceptions.
 *
 * @ingroup utils
 */
class ParserError: public Exception
{
public:
  /**
   * Constructor.
   */
  ParserError();

  /**
   * Constructor with message.
   */
  ParserError(const std::string &message);

  /**
   * Copy constructor.
   */
  ParserError(const ParserError &other);

  /**
   * Destructor.
   */
  virtual ~ParserError() throw();
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
  LexerError();

  /**
   * Constructor with message.
   */
  LexerError(const std::string &message);

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
  SyntaxError();

  /**
   * Constructor with message.
   */
  SyntaxError(const std::string &message);

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
