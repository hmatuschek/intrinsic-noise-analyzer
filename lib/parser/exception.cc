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
  : ParserError(other), _expected_tokens(other._expected_tokens)
{
  // pass...
}


SyntaxError::~SyntaxError() throw()
{
  // pass...
}


const std::set<std::string> &
SyntaxError::getExpectedTokens() const {
  return _expected_tokens;
}

void
SyntaxError::addExpectedTokenId(const std::string &id) {
  _expected_tokens.insert(id);
}

void
SyntaxError::addExpectedTokens(const std::set<std::string> &ids) {
  _expected_tokens.insert(ids.begin(), ids.end());
}
