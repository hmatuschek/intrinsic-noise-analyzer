#include <list>

#include "reaction.hh"
#include "variabledefinition.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Ast;


Reaction::Reaction(const std::string &id, KineticLaw *law, bool reversible)
  : Definition(id, Node::REACTION_DEFINITION), kinetic_law(law), is_reversible(reversible)
{
  // Done.
}


Reaction::Reaction(const std::string &id, const std::string &name, KineticLaw *law, bool reversible)
  : Definition(id, name, Node::REACTION_DEFINITION), kinetic_law(law), is_reversible(reversible)
{
  // Done.
}


Reaction::~Reaction()
{
  // first, destroy kinetic law:
  delete this->kinetic_law;
}


bool
Reaction::hasSpecies(Species *species)
{
  if (this->reactants.end() != this->reactants.find(species)) {
    return true;
  }

  if (this->products.end() != this->products.find(species)) {
    return true;
  }

  return false;
}


bool
Reaction::hasSpecies(GiNaC::symbol id)
{
  return this->hasReactant(id) || this->hasProduct(id);
}



bool
Reaction::hasReactant(Species *species)
{
  return this->reactants.end() != this->reactants.find(species);
}


bool
Reaction::hasReactant(GiNaC::symbol id)
{
  // Check all reactants:
  for (std::map<Species *, GiNaC::ex>::iterator iter=this->reactants.begin();
       iter != this->reactants.end(); iter++)
  {
    if (iter->first->getSymbol() == id) {
      return true;
    }
  }

  return false;
}



GiNaC::ex
Reaction::getReactantStoichiometry(Species *species)
{
  std::map<Species *, GiNaC::ex>::iterator item = this->reactants.find(species);

  if (this->reactants.end() == item) {
    SymbolError err;
    err << "There is no reactant " << species->getIdentifier()
        << " defined in reaction " << this->getIdentifier();
    throw err;
  }

  return item->second;
}


GiNaC::ex
Reaction::getReactantStoichiometry(GiNaC::symbol id)
{
  // Search for item in reactants table:
  for (std::map<Species *, GiNaC::ex>::iterator iter=this->reactants.begin();
       iter != this->reactants.end(); iter++)
  {
    if (iter->first->getSymbol() == id) {
      return iter->second;
    }
  }

  SymbolError err;
  err << "There is no reactant " << id << " defined in reaction " << this->getIdentifier();
  throw err;
}


void
Reaction::setReactantStoichiometry(Species *species, GiNaC::ex st)
{
  this->reactants[species] = st;
}


void
Reaction::addReactantStoichiometry(Species *species, GiNaC::ex st)
{
  // If there is already a reactant expression:
  if (this->hasReactant(species))
  {
    // Replace the stoichiometric expression with the sum of the given and present one.
    st = st + this->getReactantStoichiometry(species);
  }

  // Replace expression:
  this->setReactantStoichiometry(species, st);
}


bool
Reaction::hasProduct(Species *species)
{
  return this->products.end() != this->products.find(species);
}


bool
Reaction::hasProduct(GiNaC::symbol id)
{
  // Iterate over all products:
  for (std::map<Species *, GiNaC::ex>::iterator iter = this->products.begin();
       iter != this->products.end(); iter++)
  {
    if (iter->first->getSymbol() == id)
      return true;
  }

  return false;
}


GiNaC::ex
Reaction::getProductStoichiometry(Species *species)
{
  std::map<Species *, GiNaC::ex>::iterator item = this->products.find(species);

  if (this->products.end() != item) {
    SymbolError err;
    err << "No product " << species->getIdentifier()
        << " defined in reaction " << this->getIdentifier();
    throw err;
  }

  return item->second;
}


GiNaC::ex
Reaction::getProductStoichiometry(GiNaC::symbol id)
{
  for (std::map<Species *, GiNaC::ex>::iterator iter=this->products.begin();
       iter != this->products.end(); iter++)
  {
    if (iter->first->getSymbol() == id) {
      return iter->second;
    }
  }

  SymbolError err;
  err << "No product " << id
      << " defined in reaction " << this->getIdentifier();
  throw err;
}


void
Reaction::setProductStoichiometry(Species *species, GiNaC::ex st)
{
  this->products[species] = st;
}


void
Reaction::addProductStoichiometry(Species *species, GiNaC::ex st)
{
  // If there is already a product expression:
  if (this->hasProduct(species))
  {
    // Replace the stoichiometric expression with the sum of the given and present one.
    st = st + this->getProductStoichiometry(species);
  }

  // Replace expression:
  this->setProductStoichiometry(species, st);
}


void
Reaction::addModifier(Species *species)
{
  this->modifiers.insert(species);
}


bool
Reaction::hasModifier() const
{
  return 0 == this->modifiers.size();
}


bool
Reaction::isModifier(Species *species) const
{
  return this->modifiers.end() != this->modifiers.find(species);
}


bool
Reaction::isModifier(const GiNaC::symbol &id) const
{
  // Iterate over all modifiers:
  for (std::set<Species *>::iterator iter = this->modifiers.begin();
       iter != this->modifiers.end(); iter++)
  {
    if ((*iter)->getSymbol() == id)
      return true;
  }

  return false;
}


KineticLaw *
Reaction::getKineticLaw()
{
  return this->kinetic_law;
}


void
Reaction::setKineticLaw(KineticLaw *law)
{
  this->kinetic_law = law;
}


bool
Reaction::isReversible() const
{
  return this->is_reversible;
}


Reaction::iterator
Reaction::reacBegin()
{
  return this->reactants.begin();
}

Reaction::iterator
Reaction::reacEnd()
{
  return this->reactants.end();
}


Reaction::iterator
Reaction::prodBegin()
{
  return this->products.begin();
}

Reaction::iterator
Reaction::prodEnd()
{
  return this->products.end();
}


void
Reaction::dump(std::ostream &str)
{
  str << "Reaction: " << this->getIdentifier() << " {" << std::endl;
  // First, dump ractants of reaction:
  for (Reaction::iterator iter = this->reacBegin(); iter != this->reacEnd(); iter++)
  {
    str << iter->first << "(" << iter->second << ") + ";
  }
  str << " => ";

  // Then dump products:
  for (Reaction::iterator iter = this->prodBegin(); iter != this->prodEnd(); iter++)
  {
    str << iter->first << "(" << iter->second << ") + ";
  }
  str << std::endl << " with rate ";
  this->kinetic_law->dump(str);

  str << "}" << std::endl;
}



/* ********************************************************************************************* *
 * Implementation of kinetic law:
 * ********************************************************************************************* */
KineticLaw::KineticLaw(GiNaC::ex expr)
  : Node(Node::KINETIC_LAW), Scope(false), expression(expr)
{
  // Done.
}


KineticLaw::~KineticLaw()
{
  // The variable-definitions for the parameters are deleted by the destructor of Scope()
}


GiNaC::ex
KineticLaw::getRateLaw()
{
  return this->expression;
}


void
KineticLaw::setRateLaw(GiNaC::ex node)
{
  this->expression = node;
}


void
KineticLaw::addDefinition(Definition *def)
{
  // Check if definition is a Parameter definition
  if (! Node::isParameter(def))
  {
    InternalError err;
    err << "Only parameter-definitions can be added to a KineticLaw scope.";
    throw err;
  }

  // Add definition to scope:
  Scope::addDefinition(def);

  // Also store reference in vector:
  this->parameters.push_back(static_cast<Parameter *>(def));
}


size_t
KineticLaw::numParameters() const
{
  return this->parameters.size();
}


Parameter *
KineticLaw::getParameter(size_t i)
{
  // Simply get paramter:
  return this->parameters[i];
}


void
KineticLaw::dump(std::ostream &str)
{
  str << this->expression << " where ";
  Scope::dump(str);
}

