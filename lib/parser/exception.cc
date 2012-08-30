#include "exception.hh"

using namespace iNA::Parser;


/* ******************************************************************************************** *
 * Implementation of parser error:
 * ******************************************************************************************** */
ParserError::ParserError(int line)
  : iNA::Exception(), _line(line)
{
  // Pass...
}


ParserError::ParserError(const std::string &message, int line)
  : iNA::Exception(message), _line(line)
{
  // pass...
}


ParserError::ParserError(const ParserError &other)
  : iNA::Exception(other), _line(other._line)
{
  // pass...
}


ParserError::~ParserError() throw()
{
  // pass...
}


int ParserError::line() const { return _line; }


/* ******************************************************************************************** *
 * Implementation of lexer error:
 * ******************************************************************************************** */
LexerError::LexerError(int line)
  : ParserError(line)
{
  // Pass...
}


LexerError::LexerError(const std::string &message, int line)
  : ParserError(message, line)
{
  // pass...
}


LexerError::LexerError(const LexerError &other)
  : ParserError(other)
{
  // pass...
}


LexerError::~LexerError() throw()
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of syntax error:
 * ******************************************************************************************** */
SyntaxError::SyntaxError(int line)
  : ParserError(line)
{
  // Pass...
}


SyntaxError::SyntaxError(const std::string &message, int line)
  : ParserError(message, line)
{
  // pass...
}


SyntaxError::SyntaxError(const SyntaxError &other)
  : ParserError(other)
{
  // pass...
}


SyntaxError::~SyntaxError() throw()
{
  // pass...
}
