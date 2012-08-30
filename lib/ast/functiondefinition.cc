#include "functiondefinition.hh"
#include "exception.hh"


using namespace iNA::Ast;


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


void
FunctionDefinition::accept(Ast::Visitor &visitor) const
{
  if (FunctionDefinition::Visitor *fun_vis = dynamic_cast<FunctionDefinition::Visitor *>(&visitor)) {
    fun_vis->visit(this);
  } else {
    FunctionDefinition::traverse(visitor);
    Definition::accept(visitor);
  }
}

void
FunctionDefinition::apply(Ast::Operator &op)
{
  if (FunctionDefinition::Operator *fun_op = dynamic_cast<FunctionDefinition::Operator *>(&op)) {
    fun_op->act(this);
  } else {
    FunctionDefinition::traverse(op);
    Definition::apply(op);
  }
}

void
FunctionDefinition::traverse(Ast::Visitor &visitor) const
{
  Scope::traverse(visitor);
}

void
FunctionDefinition::traverse(Ast::Operator &op)
{
  Scope::traverse(op);
}



/* ******************************************************************************************** *
 * Implementation of "FunctionArgument" variable:
 * ******************************************************************************************** */
FunctionArgument::FunctionArgument(const std::string &id)
  : VariableDefinition(Node::FUNCTION_ARGUMENT, id, Unit::dimensionless(), false)
{
  // Pass...
}


void
FunctionArgument::accept(Ast::Visitor &visitor) const
{
  if (FunctionArgument::Visitor *arg_vis = dynamic_cast<FunctionArgument::Visitor *>(&visitor)) {
    arg_vis->visit(this);
  } else {
    VariableDefinition::accept(visitor);
  }
}


void
FunctionArgument::apply(Ast::Operator &op)
{
  if (FunctionArgument::Operator *arg_op = dynamic_cast<FunctionArgument::Operator *>(&op)) {
    arg_op->act(this);
  } else {
    VariableDefinition::apply(op);
  }
}
