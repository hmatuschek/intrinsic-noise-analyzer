#include "exception.hh"

using namespace Fluc::Utils;


/* ******************************************************************************************** *
 * Implementation of parser error:
 * ******************************************************************************************** */
ParserError::ParserError()
  : Fluc::Exception()
{
  // Pass...
}


ParserError::ParserError(const std::string &message)
  : Fluc::Exception(message)
{
  // pass...
}


ParserError::ParserError(const ParserError &other)
  : Fluc::Exception(other)
{
  // pass...
}


ParserError::~ParserError() throw()
{
  // pass...
}



/* ******************************************************************************************** *
 * Implementation of lexer error:
 * ******************************************************************************************** */
LexerError::LexerError()
  : ParserError()
{
  // Pass...
}


LexerError::LexerError(const std::string &message)
  : ParserError(message)
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
SyntaxError::SyntaxError()
  : ParserError()
{
  // Pass...
}


SyntaxError::SyntaxError(const std::string &message)
  : ParserError(message)
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
