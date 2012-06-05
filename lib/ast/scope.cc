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
Scope::Scope(bool is_closed)
  : definitions(), is_closed(is_closed)
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
  return this->definitions.end() != this->definitions.find(name);
}


Definition *
Scope::getDefinition(const std::string &name)
{
  std::map<std::string, Definition *>::iterator item = this->definitions.find(name);

  if (this->definitions.end() == item) {
    SymbolError err;
    err << "Symbol " << name << " not found in scope.";
    throw err;
  }

  return item->second;
}


Definition * const
Scope::getDefinition(const std::string &name) const
{
  std::map<std::string, Definition *>::const_iterator item = this->definitions.find(name);

  if (this->definitions.end() == item) {
    SymbolError err;
    err << "Symbol " << name << " not found in scope.";
    throw err;
  }

  return item->second;
}


bool
Scope::isClosed()
{
  return is_closed;
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
