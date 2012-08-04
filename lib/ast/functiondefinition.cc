#include "functiondefinition.hh"
#include "exception.hh"


using namespace Fluc::Ast;


FunctionDefinition::FunctionDefinition(const std::string &id, std::vector<VariableDefinition *> &argdef,
                                       GiNaC::ex body)
  : Definition(id, Node::FUNCTION_DEFINITION), Scope(0, true),
    arguments(argdef.size()), function_body(body)
{
  // get symbol for args and add argument definition to function scope:
  for (size_t i=0; i<this->arguments.size(); i++)
  {
    // First, store argument in scope:
    this->addDefinition(argdef[i]);
    this->arguments[i] = argdef[i]->getSymbol();
  }
}


FunctionDefinition::~FunctionDefinition()
{
  // Done!
}


size_t
FunctionDefinition::getNumArgs()
{
  return this->arguments.size();
}


GiNaC::symbol
FunctionDefinition::getArgByIdx(size_t i)
{
  return this->arguments[i];
}


FunctionDefinition::ArgIterator
FunctionDefinition::argsBegin()
{
  return this->arguments.begin();
}


FunctionDefinition::ArgIterator
FunctionDefinition::argsEnd()
{
  return this->arguments.end();
}


GiNaC::ex
FunctionDefinition::getBody()
{
  return this->function_body;
}


void
FunctionDefinition::setBody(GiNaC::ex body)
{
  this->function_body = body;
}


GiNaC::ex
FunctionDefinition::inlineFunction(GiNaC::exvector &args)
{
  // Check if given expressions are of same number as function arguments:
  if (this->getNumArgs() != args.size())
  {
    InternalError err;
    err << "Can not inline function" << this->getIdentifier() << ": Invalid number of arguments.";
    throw err;
  }

  // Assemble substitutions:
  GiNaC::exmap subst;
  for (size_t i=0; i<this->getNumArgs(); i++)
  {
    subst[this->getArgByIdx(i)] = args[i];
  }

  return this->function_body.subs(subst);
}


void
FunctionDefinition::dump(std::ostream &str)
{
  str << this->getIdentifier() << " = lambda (...) : ...;";
}




/* ******************************************************************************************** *
 * Implementation of "FunctionArgument" variable:
 * ******************************************************************************************** */
FunctionArgument::FunctionArgument(const std::string &id)
  : VariableDefinition(Node::FUNCTION_ARGUMENT, id, Unit::dimensionless(), false)
{
  // Pass...
}
