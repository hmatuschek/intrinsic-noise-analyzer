#include "scope.hh"
#include "reaction.hh"
#include "exception.hh"
#include "functiondefinition.hh"


using namespace iNA;
using namespace iNA::Ast;



/*
 * Implementation of Scope::iterator
 */
Scope::iterator::iterator(std::map<std::string, Definition *>::iterator inner_iter)
  : inner_iter(inner_iter)
{
  // Done.
}

Definition *
Scope::iterator::operator ->()
{
  return this->inner_iter->second;
}

Definition * &
Scope::iterator::operator *()
{
  return this->inner_iter->second;
}

Scope::iterator const
Scope::iterator::operator ++(int)
{
  return this->inner_iter++;
}

bool
Scope::iterator::operator ==(const Scope::iterator &other)
{
  return this->inner_iter == other.inner_iter;
}

bool
Scope::iterator::operator !=(const Scope::iterator &other)
{
  return this->inner_iter != other.inner_iter;
}



/*
 * Implementation of Scope::const_iterator
 */
Scope::const_iterator::const_iterator(std::map<std::string, Definition *>::const_iterator inner_iter)
  : inner_iter(inner_iter)
{
  // Done.
}

Definition * const
Scope::const_iterator::operator ->()
{
  return this->inner_iter->second;
}

Definition * const&
Scope::const_iterator::operator *()
{
  return this->inner_iter->second;
}

Scope::const_iterator const
Scope::const_iterator::operator ++(int)
{
  return this->inner_iter++;
}

bool
Scope::const_iterator::operator ==(const Scope::const_iterator &other)
{
  return this->inner_iter == other.inner_iter;
}

bool
Scope::const_iterator::operator !=(const Scope::const_iterator &other)
{
  return this->inner_iter != other.inner_iter;
}



/*
 * Implementation of Scope.
 */
Scope::Scope(Scope *parent, bool is_closed)
  : definitions(), symbol_table(), is_closed(is_closed), _parent_scope(parent)
{
  // Done.
}


Scope::~Scope()
{
  // Iterate over all definitions and free them:
  for(std::map<std::string, Definition *>::iterator iter = this->definitions.begin();
      iter != this->definitions.end(); iter++)
  {
    delete iter->second;
  }
}


bool
Scope::hasParentScope() const {
  return 0 != _parent_scope;
}

Scope *
Scope::getRootScope() {
  if (0 == _parent_scope) { return this; }

  return _parent_scope->getRootScope();
}

const Scope *
Scope::getRootScope() const {
  if (0 == _parent_scope) { return this; }

  return _parent_scope->getRootScope();
}

Scope *
Scope::getParentScope() {
  return _parent_scope;
}

const Scope *
Scope::getParentScope() const {
  return _parent_scope;
}

void
Scope::setParent(Scope *parent) {
  _parent_scope = parent;
}


void
Scope::addDefinition(Definition *def)
{
  // Check if the if there is a definition with the same identifier:
  std::map<std::string, Definition *>::iterator item;
  if (this->definitions.end() != (item = this->definitions.find(def->getIdentifier()))) {
    delete item->second;
    item->second = def;
  } else {
    // Store definition in table
    this->definitions[def->getIdentifier()] = def;
  }

  Scope *scope = 0;
  if (0 != (scope = dynamic_cast<Scope *>(def))) {
    // If the definition is a scope, set scopes parent:
    scope->setParent(this);
  }

  Reaction *reaction = 0;
  if (0 != (reaction = dynamic_cast<Reaction *>(def))) {
    // If the definition is a reaction, set parent of kinetic law:
    reaction->getKineticLaw()->setParent(this);
  }

  VariableDefinition *var = 0;
  if (0 != (var = dynamic_cast<VariableDefinition *>(def))) {
    // If the definition is a variable definition -> store symbol of variable in symbol_table.
    symbol_table[var->getSymbol()] = var;
  }
}


void
Scope::remDefinition(Definition *def)
{
  // Check if the if there is a definition with the same identifier:
  std::map<std::string, Definition *>::iterator item = this->definitions.find(def->getIdentifier());
  if (this->definitions.end() == item)
  {
    SymbolError err;
    err << "Can not remove definition " << def->getIdentifier()
        << " from scope, definition not known.";
    throw err;
  }

  this->definitions.erase(def->getIdentifier());

  if (Node::isVariableDefinition(def)) {
    symbol_table.erase(static_cast<VariableDefinition *>(def)->getSymbol());
  }
}


bool
Scope::hasDefinition(const std::string &name) const throw()
{
  if (this->definitions.end() != this->definitions.find(name))
    return true;

  if (0 != _parent_scope && !is_closed)
    return _parent_scope->hasDefinition(name);

  return false;
}


Definition *
Scope::getDefinition(const std::string &name, bool local)
{
  // Search in this scope:
  std::map<std::string, Definition *>::iterator item = this->definitions.find(name);
  if (definitions.end() != item) { return item->second; }

  // If not closed and has parent -> search in parent scope:
  if (0 != _parent_scope && !is_closed && !local)
    return _parent_scope->getDefinition(name);

  // Not found...
  SymbolError err;
  err << "Symbol " << name << " not found in scope.";
  throw err;
}


Definition * const
Scope::getDefinition(const std::string &name) const
{
  // Search in this scope:
  std::map<std::string, Definition *>::const_iterator item = this->definitions.find(name);
  if (definitions.end() != item) { return item->second; }

  // If not closed and has parent -> search in parent scope:
  if (0 != _parent_scope && !is_closed)
    return _parent_scope->getDefinition(name);

  // Not found...
  SymbolError err;
  err << "Symbol " << name << " not found in scope.";
  throw err;
}


bool
Scope::hasVariable(const GiNaC::symbol &symbol) const
{
  if (this->symbol_table.end() != this->symbol_table.find(symbol)) { return true; }
  if (! hasParentScope() || isClosed()) { return false; }
  return _parent_scope->hasVariable(symbol);
}


bool
Scope::hasVariable(const std::string &identifier) const
{
  return this->hasDefinition(identifier) && Node::isVariableDefinition(this->getDefinition(identifier));
}


VariableDefinition *
Scope::getVariable(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isVariableDefinition(def))
  {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a variable.";
    throw err;
  }

  return static_cast<Ast::VariableDefinition *>(def);
}


VariableDefinition * const
Scope::getVariable(const std::string &identifier) const
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isVariableDefinition(def)) {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a variable.";
    throw err;
  }

  return static_cast<Ast::VariableDefinition *>(def);
}


VariableDefinition *
Scope::getVariable(const GiNaC::symbol &symbol)
{
  std::map<GiNaC::symbol, VariableDefinition *, GiNaC::ex_is_less>::iterator item
      = this->symbol_table.find(symbol);

  if (this->symbol_table.end() == item) {
    if (isClosed() || ! hasParentScope()) {
      SymbolError err;
      err << "Symbol " << symbol << " does not name a variable.";
      throw err;
    }

    return _parent_scope->getVariable(symbol);
  }

  return item->second;
}


VariableDefinition * const
Scope::getVariable(const GiNaC::symbol &symbol) const
{
  std::map<GiNaC::symbol, VariableDefinition *, GiNaC::ex_is_less>::const_iterator item
      = this->symbol_table.find(symbol);

  if (this->symbol_table.end() == item) {
    if (isClosed() || !hasParentScope()) {
      SymbolError err;
      err << "Symbol " << symbol << " does not name a variable.";
      throw err;
    }

    return _parent_scope->getVariable(symbol);
  }

  return item->second;
}


bool Scope::isClosed() const { return is_closed; }


std::string
Scope::getNewIdentifier(const std::string &base_name)
{
  std::stringstream buffer(base_name);
  size_t count = 1;
  while (hasDefinition(buffer.str())) {
    buffer.str(""); buffer << base_name << "_" << count; count++;
  }
  return buffer.str();
}


Scope::iterator
Scope::begin()
{
  return Scope::iterator(this->definitions.begin());
}


Scope::const_iterator
Scope::begin() const
{
  return Scope::const_iterator(this->definitions.begin());
}


Scope::iterator
Scope::end()
{
  return Scope::iterator(this->definitions.end());
}


Scope::const_iterator
Scope::end() const
{
  return Scope::const_iterator(this->definitions.end());
}


void
Scope::dump(std::ostream &str)
{
  str << "{" << std::endl;
  for (Scope::iterator iter = this->begin(); iter != this->end(); iter++)
  {
    (*iter)->dump(str); str << std::endl;
  }
  str << "}" << std::endl;
}


void
Scope::accept(Ast::Visitor &visitor) const
{
  // Default behavior, traverse scope:
  this->traverse(visitor);
}


void
Scope::apply(Ast::Operator &op)
{
  // Default behavior, traverse scope:
  this->traverse(op);
}

void
Scope::traverse(Ast::Visitor &visitor) const
{
  for (Scope::const_iterator iter = this->begin(); iter != this->end(); iter++) {
    (*iter)->accept(visitor);
  }
}

void
Scope::traverse(Ast::Operator &op)
{
  for (Scope::iterator iter = this->begin(); iter != this->end(); iter++) {
    (*iter)->apply(op);
  }
}
