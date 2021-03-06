#include "exception.hh"

using namespace iNA;


Exception::Exception()
  : std::exception(), message()
{
  // Pass...
}


Exception::Exception(const std::string &message)
  : std::exception(), message(message)
{
  // Pass...
}


Exception::Exception(const Exception &other)
  : std::exception(), message(other.message)
{
  // Pass...
}


Exception::~Exception() throw ()
{

}


const char *
Exception::what() const throw()
{
  // Return C string of string representation.
  return this->message.c_str();
}


Exception &
Exception::operator <<(unsigned char c)
{
  std::stringstream str; str << c;
  this->message.append(str.str());

  return *this;
}


Exception &
Exception::operator <<(const char *text)
{
  this->message.append(text);
  return *this;
}


Exception &
Exception::operator <<(const std::string &text)
{
  this->message.append(text);
  return *this;
}


Exception &
Exception::operator <<(unsigned int value)
{
  std::stringstream str; str << value;
  this->message.append(str.str());
  return *this;
}


Exception &
Exception::operator <<(unsigned long value)
{
  std::stringstream str; str << value;
  this->message.append(str.str());
  return *this;
}


Exception &
Exception::operator <<(double value)
{
  std::stringstream str; str << value;
  this->message.append(str.str());
  return *this;
}


Exception &
Exception::operator <<(const GiNaC::ex &expression)
{
  std::stringstream str; str << expression;
  this->message.append(str.str());
  return *this;
}



CompilerException::CompilerException()
  : Exception()
{
  // Pass
}

CompilerException::CompilerException(const std::string &message)
  : Exception()
{
  (*this) << message;
}

CompilerException::CompilerException(const CompilerException &other)
  : Exception(other)
{
  // pass.
}

CompilerException::~CompilerException() throw()
{
  // Pass...
}




SymbolError::SymbolError()
  : Exception("Symbol Error: ")
{
  // pass
}

SymbolError::SymbolError(const std::string &message)
  : Exception("Symbol Error: ")
{
  (*this) << message;
}

SymbolError::SymbolError(const SymbolError &other)
  : Exception(other)
{
  // Pass.
}

SymbolError::~SymbolError() throw()
{
  // Pass...
}



SemanticError::SemanticError()
  : Exception("Semantic Error: ")
{
  // Pass.
}

SemanticError::SemanticError(const std::string &message)
  : Exception("Semantic Error: ")
{
  (*this) << message;
}

SemanticError::SemanticError(const SemanticError &other)
  : Exception(other)
{
  // pass.
}


SemanticError::~SemanticError() throw()
{
  // Pass...
}



TypeError::TypeError()
  : Exception("Type Error: ")
{
  // Pass.
}

TypeError::TypeError(const std::string &message)
  : Exception("Type Error: ")
{
  (*this) << message;
}

TypeError::TypeError(const TypeError &other)
  : Exception(other)
{
  // Pass.
}


TypeError::~TypeError() throw()
{
  // Pass...
}



InternalError::InternalError()
  : CompilerException("Internal: ")
{
  // pass.
}

InternalError::InternalError(const std::string &message)
  : CompilerException("Internal: ")
{
  (*this) << message;
}

InternalError::InternalError(const InternalError &other)
  : CompilerException(other)
{
  // pass.
}

InternalError::~InternalError() throw()
{
  // Pass...
}



SBMLParserError::SBMLParserError()
  : Exception("SBML Parser: ")
{
  // pass.
}

SBMLParserError::SBMLParserError(const std::string &message)
  : Exception("SBML Parser: ")
{
  (*this) << message;
}

SBMLParserError::SBMLParserError(const SBMLParserError &other)
  : Exception(other)
{
  // pass.
}

SBMLParserError::~SBMLParserError() throw()
{
  // Pass...
}



ExportError::ExportError() : Exception("Export: ") {
  // pass.
}

ExportError::ExportError(const std::string &message) : Exception("Export: ") {
  (*this) << message;
}

ExportError::ExportError(const ExportError &other) : Exception(other) {
  // pass.
}

ExportError::~ExportError() throw() {
  // Pass...
}



SBMLFeatureNotSupported::SBMLFeatureNotSupported()
  : SBMLParserError("SBML feature not supported: ")
{
  // Pass...
}


SBMLFeatureNotSupported::SBMLFeatureNotSupported(const std::string &message)
  : SBMLParserError("SBML feature not supported: ")
{
  (*this) << message;
}


SBMLFeatureNotSupported::SBMLFeatureNotSupported(const SBMLFeatureNotSupported &other)
: SBMLParserError(other)
{
  // Pass...
}


SBMLFeatureNotSupported::~SBMLFeatureNotSupported() throw()
{
  // Pass...
}


RuntimeError::RuntimeError()
  : Exception("Runtime error: ")
{
  // Pass...
}

RuntimeError::RuntimeError(const std::string &message)
  : Exception("Runtime error: ")
{
  (*this) << message;
}

RuntimeError::RuntimeError(const RuntimeError &other)
  : Exception(other)
{
  // Pass...
}

RuntimeError::~RuntimeError() throw ()
{
  // pass...
}


NumericError::NumericError()
  : RuntimeError("Numeric error: ")
{
  // Pass...
}


NumericError::NumericError(const std::string &message)
  : RuntimeError("Numeric error: ")
{
  (*this) << message;
}


NumericError::NumericError(const NumericError &other)
  : RuntimeError(other)
{
  // pass...
}


NumericError::~NumericError() throw ()
{
  // Pass...
}
