#include "model.hh"
#include "exception.hh"
#include "reaction.hh"
#include "assembler.hh"
#include "trafo/modelcopyist.hh"


using namespace Fluc;
using namespace Fluc::Ast;


Model::Model()
  : Module()
{
  // Pass...
}


Model::Model(libsbml::Model *model)
  : Module()
{
  Assembler assembler(*this);
  assembler.processModel(model);
}


Model::Model(const Model &other)
  : Module()
{
  // Copy "other" module into this module
  Trafo::ModelCopyist::copy(&other, this);
}


size_t
Model::numParameters() const
{
  return this->parameter_vector.size();
}


bool
Model::hasParameter(const std::string &name) const
{
  return this->hasDefinition(name) && Node::isParameter(this->getDefinition(name));
}

bool
Model::hasParameter(const GiNaC::symbol &symbol) const
{
  return this->hasVariable(symbol) && Node::isParameter(this->getVariable(symbol));
}


Parameter *
Model::getParameter(const std::string &name)
{
  return this->getParameter(name);
}

Parameter * const
Model::getParameter(const std::string &name) const
{
  return this->getParameter(name);
}


Parameter *
Model::getParameter(const GiNaC::symbol &symbol)
{
  Ast::VariableDefinition *def = this->getVariable(symbol);
  if (! Node::isParameter(def))
  {
    SymbolError err;
    err << "Symbol " << symbol << " is not associated with a parameter.";
    throw err;
  }

  return static_cast<Parameter *>(def);
}

Parameter * const
Model::getParameter(const GiNaC::symbol &symbol) const
{
  Ast::VariableDefinition *def = this->getVariable(symbol);
  if (! Node::isParameter(def))
  {
    SymbolError err;
    err << "Symbol " << symbol << " is not associated with a parameter.";
    throw err;
  }

  return static_cast<Parameter * const>(def);
}


Parameter *
Model::getParameter(size_t idx)
{
  return this->parameter_vector[idx];
}

Parameter * const
Model::getParameter(size_t idx) const
{
  return this->parameter_vector[idx];
}


size_t
Model::getParameterIdx(const std::string &id) const
{
  return this->getParameterIdx(this->getParameter(id));
}


size_t
Model::getParameterIdx(const GiNaC::symbol &symbol) const
{
  return this->getParameterIdx(this->getParameter(symbol));
}


size_t
Model::getParameterIdx(Parameter *parameter) const
{
  // Search vector for parameter:
  for (size_t i=0; i<this->parameter_vector.size(); i++)
  {
    if (parameter == this->parameter_vector[i])
    {
      return i;
    }
  }

  SymbolError err;
  err << "Can not get Index of Parameter " << parameter->getIdentifier()
      << ":Not defined in model.";
  throw err;
}



size_t
Model::numCompartments() const
{
  return this->compartment_vector.size();
}


bool
Model::hasCompartment(const std::string &name) const
{
  return this->hasDefinition(name) &&
      Node::isCompartment(this->getDefinition(name));
}

bool
Model::hasCompartment(const GiNaC::symbol &symbol) const
{
  return this->hasVariable(symbol) &&
      Node::isCompartment(this->getVariable(symbol));
}


Compartment *
Model::getCompartment(const std::string &name)
{
  Ast::Definition *definition = this->getDefinition(name);

  if (! Node::isCompartment(definition))
  {
    SymbolError err;
    err << "No compartment with id " << name << " defined in this model.";
    throw err;
  }

  return static_cast<Ast::Compartment *>(definition);
}

Compartment * const
Model::getCompartment(const std::string &name) const
{
  Ast::Definition * const definition = this->getDefinition(name);

  if (! Node::isCompartment(definition))
  {
    SymbolError err;
    err << "No compartment with id " << name << " defined in this model.";
    throw err;
  }

  return static_cast<Ast::Compartment * const>(definition);
}


Compartment *
Model::getCompartment(const GiNaC::symbol &symbol)
{
  Definition *def = this->getVariable(symbol);

  if (! Node::isCompartment(def))
  {
    SymbolError err;
    err << "There is no compartment associated with symbol " << symbol;
    throw err;
  }

  return static_cast<Compartment *>(def);
}

Compartment * const
Model::getCompartment(const GiNaC::symbol &symbol) const
{
  Definition * const def = this->getVariable(symbol);

  if (! Node::isCompartment(def))
  {
    SymbolError err;
    err << "There is no compartment associated with symbol " << symbol;
    throw err;
  }

  return static_cast<Compartment * const>(def);
}


Compartment *
Model::getCompartment(size_t idx)
{
  return this->compartment_vector[idx];
}

Compartment * const
Model::getCompartment(size_t idx) const
{
  return this->compartment_vector[idx];
}


size_t
Model::getCompartmentIdx(const std::string &id) const
{
  return this->getCompartmentIdx(this->getCompartment(id));
}


size_t
Model::getCompartmentIdx(const GiNaC::symbol &symbol) const
{
  return this->getCompartmentIdx(this->getCompartment(symbol));
}


size_t
Model::getCompartmentIdx(Compartment *compartment) const
{
  // Search compartment vector for compartment:
  for (size_t i=0; i<this->compartment_vector.size(); i++)
  {
    if (compartment == this->compartment_vector[i])
    {
      return i;
    }
  }

  SymbolError err;
  err << "Can not get index of compartment " << compartment->getIdentifier()
      << ": Not defined in model.";
  throw err;
}


size_t
Model::numSpecies() const
{
  return this->species_vector.size();
}


bool
Model::hasSpecies(const std::string &id) const
{
  return this->hasDefinition(id) && Node::isSpecies(this->getDefinition(id));
}


bool
Model::hasSpecies(const GiNaC::symbol &symbol) const
{
  return this->hasVariable(symbol) && Node::isSpecies(this->getVariable(symbol));
}


Species *
Model::getSpecies(const std::string &id)
{
  Definition *def = this->getDefinition(id);

  if (! Node::isSpecies(def))
  {
    SymbolError err;
    err << "There is no species named " << id << " defined in module.";
    throw err;
  }

  return static_cast<Species *>(def);
}

Species * const
Model::getSpecies(const std::string &id) const
{
  Definition * const def = this->getDefinition(id);

  if (! Node::isSpecies(def))
  {
    SymbolError err;
    err << "There is no species named " << id << " defined in module.";
    throw err;
  }

  return static_cast<Species * const>(def);
}


Species *
Model::getSpecies(const GiNaC::symbol &symbol)
{
  Definition *def = this->getVariable(symbol);

  if (! Node::isSpecies(def))
  {
    SymbolError err;
    err << "There is no species named " << symbol << " defined in module.";
    throw err;
  }

  return static_cast<Species *>(def);
}

Species * const
Model::getSpecies(const GiNaC::symbol &symbol) const
{
  Definition * const def = this->getVariable(symbol);

  if (! Node::isSpecies(def))
  {
    SymbolError err;
    err << "There is no species named " << symbol << " defined in module.";
    throw err;
  }

  return static_cast<Species * const>(def);
}


Species *
Model::getSpecies(size_t idx)
{
  return this->species_vector[idx];
}

Species * const
Model::getSpecies(size_t idx) const
{
  return this->species_vector[idx];
}


size_t
Model::getSpeciesIdx(const std::string &id) const
{
  return this->getSpeciesIdx(this->getSpecies(id));
}


size_t
Model::getSpeciesIdx(const GiNaC::symbol &symbol) const
{
  return this->getSpeciesIdx(this->getSpecies(symbol));
}


size_t
Model::getSpeciesIdx(Species *species) const
{
  // Search species vector for species:
  for (size_t i=0; i<this->species_vector.size(); i++)
  {
    if (species == this->species_vector[i])
    {
      return i;
    }
  }

  SymbolError err;
  err << "Can not get index of species " << species->getIdentifier()
      << ": Species not defined in model.";
  throw err;
}


size_t
Model::numReactions() const
{
  return this->reaction_vector.size();
}


bool
Model::hasReaction(const std::string &name) const
{
  return this->hasDefinition(name) &&
      Node::isReactionDefinition(this->getDefinition(name));
}


Reaction *
Model::getReaction(size_t idx)
{
  return this->reaction_vector[idx];
}

Reaction * const
Model::getReaction(size_t idx) const
{
  return this->reaction_vector[idx];
}


size_t
Model::getReactionIdx(const std::string &id) const
{
  return this->getReactionIdx(Module::getReaction(id));
}


size_t
Model::getReactionIdx(Reaction *reac) const
{
  // Search reaction in reaction vector:
  for (size_t i=0; i<this->reaction_vector.size(); i++)
  {
    if (reac == this->reaction_vector[i])
    {
      return i;
    }
  }

  SymbolError err;
  err << "Can not get index for reaction " << reac->getIdentifier()
      << ": Not defined in model.";
  throw err;
}


void
Model::addDefinition(Definition *def)
{
  // First, add definition to scope (takes ownership)
  Module::addDefinition(def);

  // Add definition to vectors of definitions:
  switch (def->getNodeType())
  {
  case Node::COMPARTMENT_DEFINITION:
    this->compartment_vector.push_back(static_cast<Compartment *>(def));
    break;

  case Node::SPECIES_DEFINITION:
    this->species_vector.push_back(static_cast<Species *>(def));
    break;

  case Node::PARAMETER_DEFINITION:
    this->parameter_vector.push_back(static_cast<Parameter *>(def));
    break;

  case Node::REACTION_DEFINITION:
    this->reaction_vector.push_back(static_cast<Reaction *>(def));
    break;

  default:
    break;
  }
}
