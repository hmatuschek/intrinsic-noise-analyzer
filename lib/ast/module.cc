#include "module.hh"
#include "exception.hh"
#include "reaction.hh"
#include "functiondefinition.hh"


using namespace iNA;
using namespace iNA::Ast;


Module::Module(const std::string &name)
  : Scope(0, false)
{
}


Module::~Module()
{
}


void
Module::addDefinition(Definition *def)
{
  // First, add definition to Scope:
  Scope::addDefinition(def);

  // If definition is a variable definition:
  if (Node::isVariableDefinition(def))
  {
    Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(def);
    this->symbol_table[var->getSymbol()] = var;
  }
}


void
Module::remDefinition(Definition *def)
{
  // First, add definition to Scope:
  Scope::remDefinition(def);

  // If definition is a variable definition:
  if (Node::isVariableDefinition(def))
  {
    Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(def);
    this->symbol_table.erase(var->getSymbol());
  }
}



Reaction *
Module::getReaction(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isReactionDefinition(def))
  {
    SymbolError err;
    err << "There is no reaction with identifier " << identifier << " defined in this module.";
    throw err;
  }

  return static_cast<Ast::Reaction *>(def);
}

Reaction * const
Module::getReaction(const std::string &identifier) const
{
  Ast::Definition * const def = this->getDefinition(identifier);

  if (! Node::isReactionDefinition(def))
  {
    SymbolError err;
    err << "There is no reaction with identifier " << identifier << " defined in this module.";
    throw err;
  }

  return static_cast<Ast::Reaction * const>(def);
}


FunctionDefinition *
Module::getFunction(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isFunctionDefinition(def))
  {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a function definition in this module.";
    throw err;
  }

  return static_cast<Ast::FunctionDefinition *>(def);
}
