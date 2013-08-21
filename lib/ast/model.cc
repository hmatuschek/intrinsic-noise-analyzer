#include "model.hh"
#include "exception.hh"
#include "reaction.hh"
#include "modelcopyist.hh"
#include <trafo/substitution.hh>
#include <trafo/variablescaling.hh>
#include <cmath>
#include "math.hh"
#include "utils/logger.hh"
#include "identifier.hh"

using namespace iNA;
using namespace iNA::Ast;


Ast::Model::Model(const std::string &identifier, const std::string &name)
  : Scope(), _identifier(identifier), _name(name), _species_have_substance_units(false)
{
  INA_ASSERT_IDENTIFIER(_identifier);
  // Define default units:
  _substance_unit = ScaledBaseUnit(ScaledBaseUnit::MOLE, 1, 0, 1);
  _volume_unit    = ScaledBaseUnit(ScaledBaseUnit::LITRE, 1, 0, 1);
  _area_unit      = ScaledBaseUnit(ScaledBaseUnit::METRE, 1, 0, 2);
  _length_unit    = ScaledBaseUnit(ScaledBaseUnit::METRE, 1, 0, 1);
  _time_unit      = ScaledBaseUnit(ScaledBaseUnit::SECOND, 1, 0, 1);
}


Ast::Model::Model(const Model &other)
  : Scope(), _species_have_substance_units(other._species_have_substance_units)
{
  // Copy "other" model into this model
  ModelCopyist::copy(&other, this);
}


const std::string &
Ast::Model::getIdentifier() const {
  return _identifier;
}

void
Ast::Model::setIdentifier(const std::string &identifier) {
  INA_ASSERT_IDENTIFIER(identifier);
  _identifier = identifier;
}

bool
Ast::Model::hasName() const {
  return 0 != _name.size();
}

const std::string &
Model::getName() const {
  return _name;
}

void
Model::setName(const std::string &name) {
  _name = name;
}

GiNaC::symbol
Model::getTime() const {
  return _time_symbol;
}

GiNaC::symbol
Model::getAvogadro() {
  // If the avogadro constant was not defined yet as a global paramter -> define it
  if (! hasParameter("_avogadro_const")) {
    Parameter *a = new Parameter("_avogadro_const", Unit::dimensionless(), true);
    a->setName("$N_A$");
    a->setValue(6.02214129e23);
    addDefinition(a);
  }

  return getParameter("_avogadro_const")->getSymbol();
}

bool
Model::isExplTimeDep(const GiNaC::ex &expression) const {
  return expression.has(_time_symbol);
}


bool
Model::speciesHaveSubstanceUnits() const {
  return _species_have_substance_units;
}

void
Model::setSpeciesHaveSubstanceUnits(bool has_substance_units)
{
  // If nothing changes -> skip
  if (_species_have_substance_units == has_substance_units) { return; }

  // Store
  _species_have_substance_units = has_substance_units;
  // Assemble model scaling
  Trafo::VariableScaling scaling;
  for (size_t i=0; i<numSpecies(); i++) {
    Ast::Species *species = getSpecies(i);
    if (_species_have_substance_units) {
      // Translation concentration -> substance:
      scaling.add(species->getSymbol(), species->getCompartment()->getSymbol());
    } else {
      // Translation substance -> concentration
      scaling.add(species->getSymbol(), 1./species->getCompartment()->getSymbol());
    }
  }

  // Apply variable scaleing on model:
  this->apply(scaling);
}


Unit
Model::getSpeciesUnit() const {
  if (_species_have_substance_units) {
    return getSubstanceUnit();
  }
  return getSubstanceUnit() / getVolumeUnit();
}

const Unit &
Model::getSubstanceUnit() const {
  return _substance_unit;
}

Unit
Model::getConcentrationUnit() const {
  return _substance_unit / _volume_unit;
}

void
Model::setSubstanceUnit(const Unit &unit, bool scale_model)
{
  if (! unit.isSubstanceUnit()) {
    TypeError err; std::stringstream buffer; unit.dump(buffer);
    err << "Can not set substance unit to " << buffer.str() << ": is not a substance unit.";
    throw err;
  }

  // Set default substance unit.
  Unit old_unit = _substance_unit; _substance_unit = unit;

  // skip the rest if no model scaleing
  if (! scale_model) { return; }

  // Can we scale the model from the old to the new unit?
  Unit scale = old_unit/unit; double factor;
  if (scale.isDimensionless()) {
    factor = scale.getMultiplier(); factor *= std::pow(10., scale.getScale());
  } else if (2 == scale.size() && scale.hasVariantOf(ScaledBaseUnit::MOLE, 1) &&
             scale.hasVariantOf(ScaledBaseUnit::ITEM, -1)) {
    factor = scale.getMultiplier(); factor *= std::pow(10., scale.getScale());
    factor *= constants::AVOGADRO;
  } else if (2 == scale.size() && scale.hasVariantOf(ScaledBaseUnit::MOLE, -1) &&
             scale.hasVariantOf(ScaledBaseUnit::ITEM, 1)) {
    factor = scale.getMultiplier(); factor *= std::pow(10., scale.getScale());
    factor /= constants::AVOGADRO;
  } else {
    TypeError err; std::stringstream buffer;
    err << "Can not rescale species as unit ";
    old_unit.dump(buffer); err << buffer.str() << " can not be traslated into unit ";
    buffer.str(""); unit.dump(buffer); err << buffer.str();
    throw err;
  }

  // Assemble scaling such that all species are scaled with factor:
  Trafo::VariableScaling scaling;
  for (size_t i=0; i<numSpecies(); i++) {
    scaling.add(getSpecies(i)->getSymbol(), factor);
  }

  this->apply(scaling);
}


const Unit &
Model::getVolumeUnit() const {
  return _volume_unit;
}

void
Model::setVolumeUnit(const Unit &unit, bool scale_model)
{
  // Check if unit is a volume unit
  if (! unit.isVolumeUnit()) {
    TypeError err; std::stringstream buffer; unit.dump(buffer);
    err << "Can not set volume unit to " << buffer.str() << ": is not a volume unit.";
    throw err;
  }

  // Set default volume unit.
  Unit old_unit = _volume_unit; _volume_unit = unit;

  // skip the rest if no model scaleing
  if (! scale_model) { return; }

  // Can we scale the model from the old to the new unit?
  Unit scale = old_unit/unit;
  if (! scale.isDimensionless()) {
    TypeError err; std::stringstream buffer;
    err << "Can not rescale compartments as unit ";
    old_unit.dump(buffer); err << buffer.str() << " can not be traslated into unit ";
    buffer.str(""); unit.dump(buffer); err << buffer.str();
    throw err;
  }

  // Get scaling factor
  double factor = scale.getMultiplier(); factor *= std::pow(10., scale.getScale());

  // Assemble translation table such that all compartments are scaled with factor:
  Trafo::VariableScaling scaling;
  for (size_t i=0; i<numCompartments(); i++) {
    // Get compartment:
    Compartment *compartment = getCompartment(i);
    // Process compartment only if it is 3D (volume)
    if (Compartment::VOLUME == compartment->getDimension()) {
      scaling.add(compartment->getSymbol(), factor);
    }
  }

  // Apply substitutions
  this->apply(scaling);
}


const Unit &
Model::getAreaUnit() const {
  return _area_unit;
}

void
Model::setAreaUnit(const Unit &unit, bool scale_model)
{
  if (! unit.isAreaUnit()) {
    TypeError err; std::stringstream buffer; unit.dump(buffer);
    err << "Can not set area unit to " << buffer.str() << ": is not an area unit.";
    throw err;
  }

  // Set default area unit.
  Unit old_unit = _area_unit; _area_unit = unit;

  // skip the rest if no model scaleing
  if (! scale_model) { return; }

  // Can we scale the model from the old to the new unit?
  Unit scale = old_unit/unit;
  if (! scale.isDimensionless()) {
    TypeError err; std::stringstream buffer;
    err << "Can not rescale compartments as unit ";
    old_unit.dump(buffer); err << buffer.str() << " can not be traslated into unit ";
    buffer.str(""); unit.dump(buffer); err << buffer.str();
    throw err;
  }

  // Get scaling factor
  double factor = scale.getMultiplier(); factor *= std::pow(10., scale.getScale());

  // Assemble translation table such that all compartments are scaled with factor:
  Trafo::VariableScaling scaling;
  for (size_t i=0; i<numCompartments(); i++) {
    // Get compartment:
    Compartment *compartment = getCompartment(i);
    // Process compartment only if it is 2D (area)
    if (Compartment::AREA == compartment->getDimension()) {
      scaling.add(compartment->getSymbol(), factor);
    }
  }

  // Apply substitutions
  this->apply(scaling);
}

const Unit &
Model::getLengthUnit() const {
  return _length_unit;
}

void
Model::setLengthUnit(const Unit &unit, bool scale_model)
{
  if (! unit.isLengthUnit()) {
    TypeError err; std::stringstream buffer; unit.dump(buffer);
    err << "Can not set length unit to " << buffer.str() << ": is not an length unit.";
    throw err;
  }

  // Set default length unit.
  Unit old_unit = _length_unit; _length_unit = unit;

  // skip the rest if no model scaleing
  if (! scale_model) { return; }

  // Can we scale the model from the old to the new unit?
  Unit scale = old_unit/unit;
  if (! scale.isDimensionless()) {
    TypeError err; std::stringstream buffer;
    err << "Can not rescale compartments as unit ";
    old_unit.dump(buffer); err << buffer.str() << " can not be traslated into unit ";
    buffer.str(""); unit.dump(buffer); err << buffer.str();
    throw err;
  }

  // Get scaling factor
  double factor = scale.getMultiplier(); factor *= std::pow(10., scale.getScale());

  // Assemble translation table such that all compartments are scaled with factor:
  Trafo::VariableScaling scaling;
  for (size_t i=0; i<numCompartments(); i++) {
    // Get compartment:
    Compartment *compartment = getCompartment(i);
    // Process compartment only if it is 1D (line)
    if (Compartment::LINE == compartment->getDimension()) {
      scaling.add(compartment->getSymbol(), factor);
    }
  }

  // Apply substitutions
  this->apply(scaling);
}


const Unit &
Model::getTimeUnit() const {
  return _time_unit;
}

void
Model::setTimeUnit(const Unit &unit, bool scale_model)
{
  if (! unit.isTimeUnit()) {
    TypeError err; std::stringstream buffer; unit.dump(buffer);
    err << "Can not set time unit to " << buffer.str() << ": is not an time unit.";
    throw err;
  }

  // Set unit:
  _time_unit = unit;

  if (scale_model) {
    throw InternalError("Rescaleing of model by time units is not implemented yet.");
  }
}


size_t
Model::numParameters() const {
  return this->_parameter_vector.size();
}

bool
Model::hasParameter(const std::string &name) const {
  return this->hasDefinition(name) && Node::isParameter(this->getDefinition(name));
}

bool
Model::hasParameter(const GiNaC::symbol &symbol) const {
  return this->hasVariable(symbol) && Node::isParameter(this->getVariable(symbol));
}

Parameter *
Model::getParameter(const std::string &name)
{
  // Get variable by identifier:
  Ast::VariableDefinition *def = this->getVariable(name);

  // Check if variable is a parameter:
  if (! Node::isParameter(def)) {
    SymbolError err;
    err << "Identifier " << name << " is not associated with a parameter.";
    throw err;
  }

  return static_cast<Parameter *>(def);
}

const Parameter *
Model::getParameter(const std::string &name) const
{
  const Ast::VariableDefinition *def = this->getVariable(name);
  if (! Node::isParameter(def))
  {
    SymbolError err;
    err << "Identifier " << name << " is not associated with a parameter.";
    throw err;
  }

  return static_cast<const Parameter *>(def);
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
  return this->_parameter_vector[idx];
}

Parameter * const
Model::getParameter(size_t idx) const
{
  return this->_parameter_vector[idx];
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
Model::getParameterIdx(const Parameter *parameter) const
{
  // Search vector for parameter:
  for (size_t i=0; i<this->_parameter_vector.size(); i++)
  {
    if (parameter == this->_parameter_vector[i])
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
  return this->_compartment_vector.size();
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
  return this->_compartment_vector[idx];
}

Compartment * const
Model::getCompartment(size_t idx) const
{
  return this->_compartment_vector[idx];
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
  for (size_t i=0; i<this->_compartment_vector.size(); i++)
  {
    if (compartment == this->_compartment_vector[i])
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
  return this->_species_vector.size();
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
  return this->_species_vector[idx];
}

Species * const
Model::getSpecies(size_t idx) const
{
  return this->_species_vector[idx];
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
  for (size_t i=0; i<this->_species_vector.size(); i++)
  {
    if (species == this->_species_vector[i])
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
Model::numReactions() const {
  return this->_reaction_vector.size();
}

bool
Model::hasReaction(const std::string &name) const {
  return this->hasDefinition(name) &&
      Node::isReactionDefinition(this->getDefinition(name));
}

Reaction *
Model::getReaction(const std::string &identifier)
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
Model::getReaction(const std::string &identifier) const
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

Reaction *
Model::getReaction(size_t idx) {
  return this->_reaction_vector[idx];
}

Reaction * const
Model::getReaction(size_t idx) const {
  return this->_reaction_vector[idx];
}

size_t
Model::getReactionIdx(const std::string &id) const {
  return this->getReactionIdx(getReaction(id));
}

size_t
Model::getReactionIdx(Reaction *reac) const
{
  // Search reaction in reaction vector:
  for (size_t i=0; i<this->_reaction_vector.size(); i++) {
    if (reac == this->_reaction_vector[i]) {
      return i;
    }
  }

  SymbolError err;
  err << "Can not get index for reaction " << reac->getIdentifier()
      << ": Not defined in model.";
  throw err;
}

Model::CompartmentIterator
Model::compartmentsBegin()
{
    return _compartment_vector.begin();
}

Model::CompartmentIterator
Model::compartmentsEnd()
{
    return _compartment_vector.end();
}

Model::ParameterIterator
Model::parametersBegin()
{
    return _parameter_vector.begin();
}

Model::ParameterIterator
Model::parametersEnd()
{
    return _parameter_vector.end();
}

Model::SpeciesIterator
Model::speciesBegin()
{
    return _species_vector.begin();
}

Model::SpeciesIterator
Model::speciesEnd()
{
    return _species_vector.end();
}

Model::ReactionIterator
Model::reactionsBegin()
{
    return _reaction_vector.begin();
}

Model::ReactionIterator
Model::reactionsEnd()
{
    return _reaction_vector.end();
}

FunctionDefinition *
Model::getFunction(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isFunctionDefinition(def)) {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a function definition in this module.";
    throw err;
  }

  return static_cast<Ast::FunctionDefinition *>(def);
}


void
Model::addDefinition(Definition *def)
{
  // First, add definition to scope (takes ownership)
  Scope::addDefinition(def);

  // Add definition to vectors of definitions:
  switch (def->getNodeType())
  {
  case Node::COMPARTMENT_DEFINITION:
    this->_compartment_vector.push_back(static_cast<Compartment *>(def));
    break;

  case Node::SPECIES_DEFINITION:
    this->_species_vector.push_back(static_cast<Species *>(def));
    break;

  case Node::PARAMETER_DEFINITION:
    this->_parameter_vector.push_back(static_cast<Parameter *>(def));
    break;

  case Node::REACTION_DEFINITION:
    this->_reaction_vector.push_back(static_cast<Reaction *>(def));
    break;

  default:
    break;
  }
}

void
Model::addDefinition(Definition *def, Definition *after)
{
  // First, add definition to scope (takes ownership)
  Scope::addDefinition(def);

  // Add definition to vectors of definitions:
  switch (def->getNodeType())
  {
  case Node::COMPARTMENT_DEFINITION:
  {
    std::vector<Compartment *>::iterator pos=this->_compartment_vector.begin();
    while(pos!=this->_compartment_vector.end())
        if((*(pos++))==static_cast<Compartment *>(after)) break;
    this->_compartment_vector.insert(pos,static_cast<Compartment *>(def));
  } break;

  case Node::SPECIES_DEFINITION:
  {
    std::vector<Species *>::iterator pos=this->_species_vector.begin();
    while(pos!=this->_species_vector.end())
        if((*(pos++))==static_cast<Species *>(after)) break;
    this->_species_vector.insert(pos,static_cast<Species *>(def));
  } break;

  case Node::PARAMETER_DEFINITION:
  {
    std::vector<Parameter *>::iterator pos=this->_parameter_vector.begin();
    while(pos!=this->_parameter_vector.end())
        if((*(pos++))==static_cast<Parameter *>(after)) break;
    this->_parameter_vector.insert(pos,static_cast<Parameter *>(def));
  } break;

  case Node::REACTION_DEFINITION:
  {
    std::vector<Reaction *>::iterator pos=this->_reaction_vector.begin();
    while(pos!=this->_reaction_vector.end())
        if((*(pos++))==static_cast<Reaction *>(after)) break;
    this->_reaction_vector.insert(pos,static_cast<Reaction *>(def));
  } break;

  default:
    break;
  }
}


void
Model::remDefinition(Definition *def)
{
  // Remove definition from index vectors.
  switch(def->getNodeType()) {
  case Node::COMPARTMENT_DEFINITION:
    this->_compartment_vector.erase(
          _compartment_vector.begin()+getCompartmentIdx(static_cast<Compartment *>(def)));
    break;

  case Node::SPECIES_DEFINITION:
    this->_species_vector.erase(
          _species_vector.begin()+getSpeciesIdx(static_cast<Species *>(def)));
    break;

  case Node::PARAMETER_DEFINITION:
    this->_parameter_vector.erase(
          _parameter_vector.begin()+getParameterIdx(static_cast<Parameter *>(def)));
    break;

  case Node::REACTION_DEFINITION:
    this->_reaction_vector.erase(
          _reaction_vector.begin()+getReactionIdx(static_cast<Reaction *>(def)));
    break;

  default:
    break;
  }

  // First, call parent method:
  Scope::remDefinition(def);
}


void
Model::accept(Ast::Visitor &visitor) const
{
  if (Model::Visitor *mod_vis = dynamic_cast<Model::Visitor *>(&visitor)) {
    mod_vis->visit(this);
  } else {
    Scope::accept(visitor);
  }
}


void
Model::apply(Ast::Operator &op)
{
  if (Model::Operator *mod_op = dynamic_cast<Model::Operator *>(&op)) {
    mod_op->act(this);
  } else {
    Scope::apply(op);
  }
}
