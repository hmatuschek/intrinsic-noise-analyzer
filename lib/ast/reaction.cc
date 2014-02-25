#include <list>

#include "reaction.hh"
#include "variabledefinition.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Ast;


Reaction::Reaction(const std::string &id, KineticLaw *law, bool reversible)
  : Definition(id, Node::REACTION_DEFINITION), _kinetic_law(law), _is_reversible(reversible)
{
  // Done.
}


Reaction::Reaction(const std::string &id, const std::string &name, KineticLaw *law, bool reversible)
  : Definition(id, name, Node::REACTION_DEFINITION), _kinetic_law(law), _is_reversible(reversible)
{
  // Done.
}


Reaction::~Reaction()
{
  // first, destroy kinetic law:
  delete this->_kinetic_law;
}


bool
Reaction::hasSpecies(Species *species)
{
  if (this->_reactants.end() != this->_reactants.find(species)) {
    return true;
  }

  if (this->_products.end() != this->_products.find(species)) {
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
Reaction::hasReactant(Species *species) const
{
  return this->_reactants.end() != this->_reactants.find(species);
}


bool
Reaction::hasReactant(GiNaC::symbol id) const
{
  // Check all reactants:
  for (std::map<Species *, GiNaC::ex>::const_iterator iter=this->_reactants.begin();
       iter != this->_reactants.end(); iter++)
  {
    if (iter->first->getSymbol() == id) { return true; }
  }

  return false;
}


size_t
Reaction::numReactants() const {
  return _reactants.size();
}

GiNaC::ex
Reaction::getReactantStoichiometry(Species *species)
{
  std::map<Species *, GiNaC::ex>::iterator item = this->_reactants.find(species);

  if (this->_reactants.end() == item) {
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
  for (std::map<Species *, GiNaC::ex>::iterator iter=this->_reactants.begin();
       iter != this->_reactants.end(); iter++)
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
  // Setting stoichiometry to 0 means removing it from the reaction.
  if (0 == st) {
    if (this->hasReactant(species)) {
      _reactants.erase(species);
    }
  } else {
    this->_reactants[species] = st;
  }
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


void
Reaction::clearReactants() {
  _reactants.clear();
}


bool
Reaction::hasProduct(Species *species) const  {
  return this->_products.end() != this->_products.find(species);
}


bool
Reaction::hasProduct(GiNaC::symbol id) const
{
  // Iterate over all products:
  for (std::map<Species *, GiNaC::ex>::const_iterator iter = this->_products.begin();
       iter != this->_products.end(); iter++)
  {
    if (iter->first->getSymbol() == id)
      return true;
  }

  return false;
}


size_t
Reaction::numProducts() const {
  return _products.size();
}


GiNaC::ex
Reaction::getProductStoichiometry(Species *species)
{
  std::map<Species *, GiNaC::ex>::iterator item = this->_products.find(species);

  if (this->_products.end() != item) {
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
  for (std::map<Species *, GiNaC::ex>::iterator iter=this->_products.begin();
       iter != this->_products.end(); iter++)
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
  if (0 == st) {
    if (hasProduct(species)) {
      _products.erase(species);
    }
  } else {
    this->_products[species] = st;
  }
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
Reaction::clearProducts() {
  _products.clear();
}


bool
Reaction::isModifier(Species *species) const
{
  // If species is a reactant -> false
  if (this->hasReactant(species)) { return false; }
  // If species is a reactant -> false
  if (this->hasProduct(species)) { return false; }
  // If propensity depends on species  -> true
  return this->_kinetic_law->getRateLaw().has(species->getSymbol());
}


bool
Reaction::isModifier(const GiNaC::symbol &species) const
{
  // If species is a reactant -> false
  if (this->hasReactant(species)) { return false; }
  // If species is a reactant -> false
  if (this->hasProduct(species)) { return false; }
  // If propensity depends on species  -> true
  return this->_kinetic_law->getRateLaw().has(species);
}


KineticLaw *
Reaction::getKineticLaw() const
{
  return this->_kinetic_law;
}


void
Reaction::setKineticLaw(KineticLaw *law)
{
  this->_kinetic_law = law;
}


bool
Reaction::isReversible() const
{
  return this->_is_reversible;
}

void
Reaction::setReversible(bool val)
{
  this->_is_reversible=val;
}

bool
Reaction::isReverseOf(Reaction * other)
{
    return ( (this->_reactants == other->_products) && (this->_products == other->_reactants) );
}

Reaction::iterator
Reaction::reactantsBegin()
{
  return this->_reactants.begin();
}

Reaction::iterator
Reaction::reactantsEnd()
{
  return this->_reactants.end();
}

Reaction::const_iterator
Reaction::reactantsBegin() const
{
  return this->_reactants.begin();
}

Reaction::const_iterator
Reaction::reactantsEnd() const
{
  return this->_reactants.end();
}


Reaction::iterator
Reaction::productsBegin()
{
  return this->_products.begin();
}

Reaction::iterator
Reaction::productsEnd()
{
  return this->_products.end();
}

Reaction::const_iterator
Reaction::productsBegin() const
{
  return this->_products.begin();
}

Reaction::const_iterator
Reaction::productsEnd() const
{
  return this->_products.end();
}


void
Reaction::dump(std::ostream &str)
{
  str << "Reaction: " << this->getIdentifier() << " {" << std::endl;
  // First, dump ractants of reaction:
  for (Reaction::iterator iter = this->reactantsBegin(); iter != this->reactantsEnd(); iter++)
  {
    if (1 == iter->second) {
      str << iter->first->getSymbol();
    } else {
      str << iter->second << " " << iter->first->getSymbol() << " + ";
    }
  }
  str << " => ";

  // Then dump products:
  for (Reaction::iterator iter = this->productsBegin(); iter != this->productsEnd(); iter++)
  {
    if (1 == iter->second) {
      str << iter->first->getSymbol() << " + ";
    } else {
      str << iter->second << " " << iter->first->getSymbol() << " + ";
    }
  }
  str << std::endl << " with rate ";
  this->_kinetic_law->dump(str);

  str << "}" << std::endl;
}


void
Reaction::accept(Ast::Visitor &visitor) const
{
  if (Reaction::Visitor *reac_vis = dynamic_cast<Reaction::Visitor *>(&visitor)) {
    reac_vis->visit(this);
  } else {
    this->traverse(visitor);
    Definition::accept(visitor);
  }
}


void
Reaction::apply(Ast::Operator &op)
{
  if (Reaction::Operator *reac_op = dynamic_cast<Reaction::Operator *>(&op)) {
    reac_op->act(this);
  } else {
    this->traverse(op);
    Definition::apply(op);
  }
}


void
Reaction::traverse(Ast::Visitor &visitor) const
{
  _kinetic_law->accept(visitor);
}

void
Reaction::traverse(Ast::Operator &op)
{
  _kinetic_law->apply(op);
}



/* ********************************************************************************************* *
 * Implementation of kinetic law:
 * ********************************************************************************************* */
KineticLaw::KineticLaw(GiNaC::ex expr)
  : Node(Node::KINETIC_LAW), Scope(0, false), _expression(expr)
{
  // Done.
}


KineticLaw::~KineticLaw()
{
  // The variable-definitions for the parameters are deleted by the destructor of Scope()
}


GiNaC::ex
KineticLaw::getRateLaw() const
{
  return this->_expression;
}


void
KineticLaw::setRateLaw(GiNaC::ex node)
{
  this->_expression = node;
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
  _parameters.push_back(static_cast<Parameter *>(def));
}


void
KineticLaw::remDefinition(Definition *def)
{
  // Search parameter in vector of paramters:
  std::vector<Ast::Parameter *>::iterator item = _parameters.begin();
  for (; item!=_parameters.end(); item++) {
    if ((*item) == def) { break; }
  }

  if (item == _parameters.end()) {
    InternalError err;
    err << "Can not remove paramter: " << def->getIdentifier() << " is not defined in "
        << "kinetic law.";
    throw err;
  }

  // Otherwise reomve locally:
  Scope::remDefinition(def);
  // remove parameter from vector:
  _parameters.erase(item);
}


void
KineticLaw::cleanUpParameters()
{
  // Collect all local paramters that are not used within the kinetic law expression
  std::list<Parameter *> _unused_paramters;
  for(std::vector<Parameter *>::iterator param=_parameters.begin(); param!=_parameters.end(); param++)
  {
    if(! this->getRateLaw().has((*param)->getSymbol())) {
      _unused_paramters.push_back(*param);
    }
  }

  // Remove them properly from the kinetic law:
  for (std::list<Parameter *>::iterator param=_unused_paramters.begin(); param != _unused_paramters.end(); param++)
  {
    remDefinition(*param);
  }
}


size_t
KineticLaw::numParameters() const
{
  return this->_parameters.size();
}


Parameter *
KineticLaw::getParameter(size_t i)
{
  // Simply get paramter:
  return this->_parameters[i];
}

Parameter *
KineticLaw::getParameter(const std::string &identifier)
{
  VariableDefinition *var = this->getVariable(identifier);
  if (! Node::isParameter(var)) {
    SymbolError err;
    err << "Can not resolve paramter " << identifier << ": Not defined.";
    throw err;
  }
  return static_cast<Ast::Parameter *>(var);
}


void
KineticLaw::dump(std::ostream &str)
{
  str << this->_expression << " where ";
  Scope::dump(str);
}


void
KineticLaw::accept(Ast::Visitor &visitor) const
{
  if (KineticLaw::Visitor *law_vis = dynamic_cast<KineticLaw::Visitor *>(&visitor)) {
    law_vis->visit(this);
  } else {
    this->traverse(visitor);
    Node::accept(visitor);
  }
}


void
KineticLaw::apply(Ast::Operator &op)
{
  if (KineticLaw::Operator *law_op = dynamic_cast<KineticLaw::Operator *>(&op)) {
    law_op->act(this);
  } else {
    this->traverse(op);
    Node::apply(op);
  }
}


void
KineticLaw::traverse(Ast::Visitor &visitor) const
{
  Scope::traverse(visitor);
}

void
KineticLaw::traverse(Ast::Operator &op)
{
  Scope::traverse(op);
}
