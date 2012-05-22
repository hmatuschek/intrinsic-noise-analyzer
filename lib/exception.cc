#include "exception.hh"

using namespace Fluc;


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


/*Exception &
Exception::operator <<(size_t value)
{
  std::stringstream str; str << value;
  this->message.append(str.str());
  return *this;
}*/


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
  : Exception("Compiler: ")
{
  // Pass
}

CompilerException::CompilerException(const std::string &message)
  : Exception("Compiler: ")
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
  : CompilerException("Symbol Error: ")
{
  // pass
}

SymbolError::SymbolError(const std::string &message)
  : CompilerException("Symbol Error: ")
{
  (*this) << message;
}

SymbolError::SymbolError(const SymbolError &other)
  : CompilerException(other)
{
  // Pass.
}

SymbolError::~SymbolError() throw()
{
  // Pass...
}



SemanticError::SemanticError()
  : CompilerException("Semantic Error: ")
{
  // Pass.
}

SemanticError::SemanticError(const std::string &message)
  : CompilerException("Semantic Error: ")
{
  (*this) << message;
}

SemanticError::SemanticError(const SemanticError &other)
  : CompilerException(other)
{
  // pass.
}


SemanticError::~SemanticError() throw()
{
  // Pass...
}



TypeError::TypeError()
  : CompilerException("Type Error: ")
{
  // Pass.
}

TypeError::TypeError(const std::string &message)
  : CompilerException("Type Error: ")
{
  (*this) << message;
}

TypeError::TypeError(const TypeError &other)
  : CompilerException(other)
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



SBMLFeatureNotSupported::SBMLFeatureNotSupported()
  : SBMLParserError("SBML featrue not supported: ")
{
  // Pass...
}


SBMLFeatureNotSupported::SBMLFeatureNotSupported(const std::string &message)
  : SBMLParserError("SBML featrue not supported: ")
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
