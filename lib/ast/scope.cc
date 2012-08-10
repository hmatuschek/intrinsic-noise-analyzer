#include "scope.hh"
#include "reaction.hh"
#include "exception.hh"
#include "functiondefinition.hh"


using namespace Fluc::Ast;



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
  : definitions(), is_closed(is_closed), _parent_scope(parent)
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
Scope::getDefinition(const std::string &name)
{
  // Search in this scope:
  std::map<std::string, Definition *>::iterator item = this->definitions.find(name);
  if (definitions.end() != item) { return item->second; }

  // If not closed and has parent -> search in parent scope:
  if (0 != _parent_scope && !is_closed)
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


bool Scope::isClosed() { return is_closed; }


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
