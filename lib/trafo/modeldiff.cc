#include "modeldiff.hh"
#include <ast/reaction.hh>
#include <parser/expr/parser.hh>
#include "referencecounter.hh"


using namespace iNA;
using namespace iNA::Trafo;



/* ********************************************************************************************* *
 * Implementation model history.
 * ********************************************************************************************* */
ModelHistory::ModelHistory()
  : ModelDiffGroup(), _current_index(0)
{
  // Pass...
}


ModelHistory::~ModelHistory()
{
  // Pass...
}


bool
ModelHistory::canUndo(const Ast::Model &model)
{
  // can not undo anything if there was no modification:
  if (0 == _current_index) { return false; }
  // check if last modification can be undone:
  return _items[_current_index-1]->canUndo(model);
}


bool
ModelHistory::canRedo(const Ast::Model &model)
{
  // can not redo anything was no redo
  if ((0 == _items.size()) || (_items.size() == _current_index)) { return false; }
  // check if next modification can be redone:
  return _items[_current_index]->canUndo(model);
}


void
ModelHistory::undo(Ast::Model &model)
{
  // Check if modification can be undone
  if (! canUndo(model)) {
    iNA::InternalError err("Can not undo modification!");
    throw err;
  }
  // undo
  _items[_current_index-1]->undo(model); _current_index--;
}


void
ModelHistory::redo(Ast::Model &model)
{
  // Check if modification can be redone:
  if (! canUndo(model)) {
    iNA::InternalError err("Can not redo modification!");
    throw err;
  }
  // redo
  _items[_current_index]->redo(model); _current_index++;
}


void
ModelHistory::addModification(ModelDiffItem *item)
{
  // Clear all "old" modifications
  if (_items.size() != _current_index) {
    for (size_t i=_current_index; i<_items.size(); i++) {
      delete _items[i];
    }
    _items.resize(_current_index);
  }
  // append new modification item
  ModelDiffGroup::addModification(item);
}



/* ********************************************************************************************* *
 * Implementation model diff group
 * ********************************************************************************************* */
ModelDiffGroup::ModelDiffGroup()
  : ModelDiffItem(), _items()
{
  // pass...
}


ModelDiffGroup::ModelDiffGroup(ModelDiffItem *item)
  : ModelDiffItem(), _items()
{
  _items.push_back(item);
}


ModelDiffGroup::~ModelDiffGroup()
{
  for (size_t i=0; i<_items.size(); i++) {
    delete _items[i];
  }
  _items.clear();
}


bool
ModelDiffGroup::canUndo(const Ast::Model &model)
{
  // check if all items can be undone
  for (size_t i=0; i<_items.size(); i++) {
    if (! _items[i]->canUndo(model)) { return false; }
  }
  return true;
}


bool
ModelDiffGroup::canRedo(const Ast::Model &model)
{
  // check if all items can be redone
  for (size_t i=0; i<_items.size(); i++) {
    if (! _items[i]->canRedo(model)) { return false; }
  }
  return true;
}


void
ModelDiffGroup::undo(Ast::Model &model)
{
  // Undo modifications in reverse order:
  for (int i=(int(_items.size())-1); i>=0; i--) {
    _items[i]->undo(model);
  }
}


void
ModelDiffGroup::redo(Ast::Model &model)
{
  // Redo modifications in forward order:
  for (size_t i=0; i<_items.size(); i++) {
    _items[i]->redo(model);
  }
}


void
ModelDiffGroup::addModification(ModelDiffItem *item)
{
  _items.push_back(item);
}



/* ********************************************************************************************* *
 * Implementation generic variable modification
 * ********************************************************************************************* */
VariableReferenceItem::VariableReferenceItem(const std::string &id)
  : ModelDiffItem(), _var_id(id), _parent_id("")
{
  // Pass...
}


VariableReferenceItem::VariableReferenceItem(const std::string &id, const std::string &parent_id)
  : ModelDiffItem(), _var_id(id), _parent_id(parent_id)
{
  // Pass...
}


VariableReferenceItem::~VariableReferenceItem()
{
  // pass...
}


bool
VariableReferenceItem::canUndo(const Ast::Model &model)
{
  // can be undone if the referenced variable exists:
  if (0 == _parent_id.size())
    return _hasVariable(_var_id, model);
  return _hasVariable(_var_id, _parent_id, model);
}

bool
VariableReferenceItem::canRedo(const Ast::Model &model)
{
  // can be redone if the referenced variable exists:
  if (0 == _parent_id.size())
    return _hasVariable(_var_id, model);
  return _hasVariable(_var_id, _parent_id, model);
}


bool
VariableReferenceItem::_hasVariable(const std::string &id, const Ast::Model &model)
{
  return model.hasVariable(id);
}

bool
VariableReferenceItem::_hasVariable(const std::string &id, const std::string &parent,
                                    const Ast::Model &model)
{
  if (! model.hasReaction(parent)) { return false; }
  Ast::Reaction *reac = model.getReaction(parent);
  return reac->getKineticLaw()->hasVariable(id);
}


Ast::VariableDefinition *
VariableReferenceItem::_getVariable(const std::string &id, const Ast::Model &model)
{
  return model.getVariable(id);
}

Ast::VariableDefinition *
VariableReferenceItem::_getVariable(const std::string &id, const std::string &parent, const Ast::Model &model)
{
  Ast::Reaction *reac = model.getReaction(parent);
  return reac->getKineticLaw()->getVariable(id);
}



/* ********************************************************************************************* *
 * Implementation set variable identifier modification
 * ********************************************************************************************* */
SetVariableIdentifierItem::SetVariableIdentifierItem(const std::string &id, const std::string &new_id)
  : VariableReferenceItem(id), _new_identifier(new_id)
{
  // Pass...
}

SetVariableIdentifierItem::SetVariableIdentifierItem(const std::string &id, const std::string &new_id,
                                                     const std::string &parent_id)
  : VariableReferenceItem(id, parent_id), _new_identifier(new_id)
{
  // Pass...
}

SetVariableIdentifierItem::~SetVariableIdentifierItem()
{
  // pass...
}


bool
SetVariableIdentifierItem::canUndo(const Ast::Model &model)
{
  // can be undone if the new identifier is defined in model and the old is not
  if (0 == _parent_id.size())
    return _hasVariable(_new_identifier, model) && (! _hasVariable(_var_id, model));
  return _hasVariable(_new_identifier, _parent_id, model) && (! _hasVariable(_var_id, _parent_id, model));
}

bool
SetVariableIdentifierItem::canRedo(const Ast::Model &model)
{
  // can be redone if the new old identifier is defined in model and the new is not
  if (0 == _parent_id.size())
    return _hasVariable(_var_id, model) && (! _hasVariable(_new_identifier, model));
  return _hasVariable(_var_id, _parent_id, model) && (! _hasVariable(_new_identifier, _parent_id, model));
}


void
SetVariableIdentifierItem::undo(Ast::Model &model)
{
  // get variable
  if (0 == _parent_id.size()) {
    // Reset to "old" id:
    model.resetIdentifier(_new_identifier, _var_id);
  } else {
    // Reset to "old" id:
    Ast::Reaction *reac = model.getReaction(_parent_id);
    reac->getKineticLaw()->resetIdentifier(_new_identifier, _var_id);
  }
}

void
SetVariableIdentifierItem::redo(Ast::Model &model)
{
  // get variable
  if (0 == _parent_id.size()) {
    // Reset to "old" id:
    model.resetIdentifier(_var_id, _new_identifier);
  } else {
    // Reset to "old" id:
    Ast::Reaction *reac = model.getReaction(_parent_id);
    reac->getKineticLaw()->resetIdentifier(_var_id, _new_identifier);
  }
}



/* ********************************************************************************************* *
 * Implementation set variable name modification
 * ********************************************************************************************* */
SetVariableNameItem::SetVariableNameItem(const std::string &id, const std::string &old_name,
                                         const std::string &new_name)
  : VariableReferenceItem(id), _old_name(old_name), _new_name(new_name)
{
  // pass...
}

SetVariableNameItem::SetVariableNameItem(const std::string &id, const std::string &parent_id,
                                         const std::string &old_name, const std::string &new_name)
  : VariableReferenceItem(id, parent_id), _old_name(old_name), _new_name(new_name)
{
  // pass...
}

SetVariableNameItem::~SetVariableNameItem()
{
  // Pass...
}


bool
SetVariableNameItem::canUndo(const Ast::Model &model)
{
  return VariableReferenceItem::canUndo(model);
}

bool
SetVariableNameItem::canRedo(const Ast::Model &model)
{
  return VariableReferenceItem::canRedo(model);
}


void
SetVariableNameItem::undo(Ast::Model &model)
{
  Ast::VariableDefinition *variable=0;
  if (0 == _parent_id.size()) { variable = _getVariable(_var_id, model); }
  else { variable = _getVariable(_var_id, _parent_id, model); }
  variable->setName(_old_name);
}

void
SetVariableNameItem::redo(Ast::Model &model)
{
  Ast::VariableDefinition *variable=0;
  if (0 == _parent_id.size()) { variable = _getVariable(_var_id, model); }
  else { variable = _getVariable(_var_id, _parent_id, model); }
  variable->setName(_new_name);
}



/* ********************************************************************************************* *
 * Implementation set variable const flag modification
 * ********************************************************************************************* */
SetVariableConstFlagItem::SetVariableConstFlagItem(const std::string &id, bool old_state,
                                                   bool new_state)
  : VariableReferenceItem(id), _old_state(old_state), _new_state(new_state)
{
  // pass...
}

SetVariableConstFlagItem::SetVariableConstFlagItem(const std::string &id, const std::string &parent_id,
                                                   bool old_state, bool new_state)
  : VariableReferenceItem(id, parent_id), _old_state(old_state), _new_state(new_state)
{
  // pass...
}

SetVariableConstFlagItem::~SetVariableConstFlagItem()
{
  // Pass...
}


bool
SetVariableConstFlagItem::canUndo(const Ast::Model &model)
{
  return VariableReferenceItem::canUndo(model);
}

bool
SetVariableConstFlagItem::canRedo(const Ast::Model &model)
{
  return VariableReferenceItem::canRedo(model);
}


void
SetVariableConstFlagItem::undo(Ast::Model &model)
{
  Ast::VariableDefinition *variable=0;
  if (0 == _parent_id.size()) { variable = _getVariable(_var_id, model); }
  else { variable = _getVariable(_var_id, _parent_id, model); }
  variable->setConst(_old_state);
}

void
SetVariableConstFlagItem::redo(Ast::Model &model)
{
  Ast::VariableDefinition *variable=0;
  if (0 == _parent_id.size()) { variable = _getVariable(_var_id, model); }
  else { variable = _getVariable(_var_id, _parent_id, model); }
  variable->setConst(_new_state);
}




/* ********************************************************************************************* *
 * Implementation set variable value modification
 * ********************************************************************************************* */
SetVariableValueItem::SetVariableValueItem(const std::string &id, const std::string &old_value,
                                           const std::string &new_value)
  : VariableReferenceItem(id), _old_value(old_value), _new_value(new_value)
{
  // pass...
}

SetVariableValueItem::SetVariableValueItem(const std::string &id, const std::string &parent_id,
                                           const std::string &old_value, const std::string &new_value)
  : VariableReferenceItem(id, parent_id), _old_value(old_value), _new_value(new_value)
{
  // pass...
}

SetVariableValueItem::~SetVariableValueItem()
{
  // Pass...
}


bool
SetVariableValueItem::canUndo(const Ast::Model &model)
{
  // check if variable exists:
  if (! VariableReferenceItem::canUndo(model)) { return false; }
  // the old value can be set, if it can be parsed in its context
  if (0 != _parent_id.size()) {
    iNA::Parser::Expr::ScopeContext context(model.getReaction(_parent_id)->getKineticLaw());
    try { iNA::Parser::Expr::parseExpression(_old_value, context); }
    catch (iNA::Exception &error) { return false; }
    return true;
  }
  iNA::Parser::Expr::ScopeContext context(&model);
  try { iNA::Parser::Expr::parseExpression(_old_value, context); }
  catch (iNA::Exception &error) { return false; }
  return true;
}

bool
SetVariableValueItem::canRedo(const Ast::Model &model)
{
  // check if variable exists:
  if (! VariableReferenceItem::canUndo(model)) { return false; }
  // the new value can be set, if it can be parsed in its context
  if (0 != _parent_id.size()) {
    iNA::Parser::Expr::ScopeContext context(model.getReaction(_parent_id)->getKineticLaw());
    try { iNA::Parser::Expr::parseExpression(_new_value, context); }
    catch (iNA::Exception &error) { return false; }
    return true;
  }
  iNA::Parser::Expr::ScopeContext context(&model);
  try { iNA::Parser::Expr::parseExpression(_new_value, context); }
  catch (iNA::Exception &error) { return false; }
  return true;
}


void
SetVariableValueItem::undo(Ast::Model &model)
{
  // Get Variable
  Ast::VariableDefinition *variable=0;
  if (0 == _parent_id.size()) { variable = _getVariable(_var_id, model); }
  else { variable = _getVariable(_var_id, _parent_id, model); }

  // the old value can be set, if it can be parsed in its context
  Ast::Scope *context = &model;
  if (0 != _parent_id.size()) { context = model.getReaction(_parent_id)->getKineticLaw(); }
  GiNaC::ex value = iNA::Parser::Expr::parseExpression(_old_value, context);
  variable->setValue(value);
}


void
SetVariableValueItem::redo(Ast::Model &model)
{
  // Get Variable
  Ast::VariableDefinition *variable=0;
  if (0 == _parent_id.size()) { variable = _getVariable(_var_id, model); }
  else { variable = _getVariable(_var_id, _parent_id, model); }

  // the new value can be set, if it can be parsed in its context
  Ast::Scope *context = &model;
  if (0 != _parent_id.size()) { context = model.getReaction(_parent_id)->getKineticLaw(); }
  GiNaC::ex value = iNA::Parser::Expr::parseExpression(_new_value, context);
  variable->setValue(value);
}



/* ********************************************************************************************* *
 * Implementation set species compartment item
 * ********************************************************************************************* */
SetSpeciesCompartmentItem::SetSpeciesCompartmentItem(const std::string &id, const std::string &old_compartment,
                                                     const std::string &new_compartment)
  : VariableReferenceItem(id), _old_compartment(old_compartment), _new_compartment(new_compartment)
{
  // pass...
}

SetSpeciesCompartmentItem::~SetSpeciesCompartmentItem()
{
  // Pass...
}


bool
SetSpeciesCompartmentItem::canUndo(const Ast::Model &model)
{
  // check if variable exists:
  if (! model.hasSpecies(_var_id)) { return false; }
  // check if old compartment exsists:
  if (! model.hasCompartment(_old_compartment)) { return false; }
  return true;
}

bool
SetSpeciesCompartmentItem::canRedo(const Ast::Model &model)
{
  // check if variable exists:
  if (! model.hasSpecies(_var_id)) { return false; }
  // check if new compartment exsists:
  if (! model.hasCompartment(_new_compartment)) { return false; }
  return true;
}


void
SetSpeciesCompartmentItem::undo(Ast::Model &model)
{
  // Get Variable
  Ast::Species *species = model.getSpecies(_var_id);
  // set compartment
  species->setCompartment(model.getCompartment(_old_compartment));
}


void
SetSpeciesCompartmentItem::redo(Ast::Model &model)
{
  // Get Variable
  Ast::Species *species = model.getSpecies(_var_id);
  // set compartment
  species->setCompartment(model.getCompartment(_new_compartment));
}




/* ********************************************************************************************* *
 * Implementation set parameter unit item
 * ********************************************************************************************* */
SetParameterUnitItem::SetParameterUnitItem(const std::string &id, const Ast::Unit &old_unit,
                                           const Ast::Unit & new_unit)
  : VariableReferenceItem(id), _old_unit(old_unit), _new_unit(new_unit)
{
  // pass...
}

SetParameterUnitItem::SetParameterUnitItem(const std::string &id, const std::string &parent_id,
                                           const Ast::Unit &old_unit, const Ast::Unit &new_unit)
  : VariableReferenceItem(id, parent_id), _old_unit(old_unit), _new_unit(new_unit)
{
  // pass...
}

SetParameterUnitItem::~SetParameterUnitItem()
{
  // Pass...
}


bool
SetParameterUnitItem::canUndo(const Ast::Model &model)
{
  return model.hasParameter(_var_id);
}

bool
SetParameterUnitItem::canRedo(const Ast::Model &model)
{
  return model.hasParameter(_var_id);
}


void
SetParameterUnitItem::undo(Ast::Model &model)
{
  // Get parameter
  Ast::Parameter *param = model.getParameter(_var_id);
  // set unit:
  param->setUnit(_old_unit);
}

void
SetParameterUnitItem::redo(Ast::Model &model)
{
  // Get parameter
  Ast::Parameter *param = model.getParameter(_var_id);
  // set unit:
  param->setUnit(_new_unit);
}




/* ********************************************************************************************* *
 * Implementation full variable item
 * ********************************************************************************************* */
FullVariableItem::FullVariableItem(Ast::VariableDefinition *var, const Ast::Model &parent)
  : VariableReferenceItem(var->getIdentifier()),
    _name(var->getName()), _value(""), _is_constant(var->isConst())
{
  // Serialize expression
  std::stringstream buffer;
  Parser::Expr::serializeExpression(var->getValue(), buffer, &parent);
  _value = buffer.str();
}

FullVariableItem::FullVariableItem(Ast::VariableDefinition *var, const Ast::Reaction *parent)
  : VariableReferenceItem(var->getIdentifier(), parent->getIdentifier()),
    _name(var->getName()), _value(""), _is_constant(var->isConst())
{
  // Serialize expression
  std::stringstream buffer;
  Parser::Expr::serializeExpression(var->getValue(), buffer, parent->getKineticLaw());
  _value = buffer.str();
}

FullVariableItem::~FullVariableItem()
{
  // Pass...
}




/* ********************************************************************************************* *
 * Implementation add species item
 * ********************************************************************************************* */
AddSpeciesItem::AddSpeciesItem(Ast::Species *species, const Ast::Model &parent)
  : FullVariableItem(species, parent), _compartment(species->getCompartment()->getIdentifier())
{
  // Pass...
}

AddSpeciesItem::~AddSpeciesItem()
{
  // Pass...
}


bool
AddSpeciesItem::canUndo(const Ast::Model &model)
{
  // A species can be removed if it exists
  if (! model.hasSpecies(_var_id)) { return false; }
  Ast::Species *species = model.getSpecies(_var_id);
  // and is not referenced
  if (0 != ReferenceCounter::count(species, model)) { return false; }
  return true;
}

bool
AddSpeciesItem::canRedo(const Ast::Model &model) {
  // A species can be created if there is no definition with the same identifier:
  if (model.hasDefinition(_var_id)) { return false; }
  // and if the referenced compartment exists:
  if (! model.hasCompartment(_compartment)) { return false; }
  return true;
}


void
AddSpeciesItem::undo(Ast::Model &model)
{
  Ast::Species *species = model.getSpecies(_var_id);
  model.remDefinition(species);
}

void
AddSpeciesItem::redo(Ast::Model &model)
{
  // Get referenced compartment
  Ast::Compartment *compartment = model.getCompartment(_compartment);
  // Parse expression in global context
  GiNaC::ex value = Parser::Expr::parseExpression(_value, &model);
  // Create species
  Ast::Species *species = new Ast::Species(_var_id, value, compartment, _name, _is_constant);
  // Add to model
  model.addDefinition(species);
}




/* ********************************************************************************************* *
 * Implementation rem species item
 * ********************************************************************************************* */
RemSpeciesItem::RemSpeciesItem(Ast::Species *species, const Ast::Model &parent)
  : FullVariableItem(species, parent), _compartment(species->getCompartment()->getIdentifier())
{
  // Pass...
}

RemSpeciesItem::~RemSpeciesItem()
{
  // Pass...
}


bool
RemSpeciesItem::canRedo(const Ast::Model &model)
{
  // A species can be removed if it exists
  if (! model.hasSpecies(_var_id)) { return false; }
  Ast::Species *species = model.getSpecies(_var_id);
  // and is not referenced
  if (0 != ReferenceCounter::count(species, model)) { return false; }
  return true;
}

bool
RemSpeciesItem::canUndo(const Ast::Model &model) {
  // A species can be created if there is no definition with the same identifier:
  if (model.hasDefinition(_var_id)) { return false; }
  // and if the referenced compartment exists:
  if (! model.hasCompartment(_compartment)) { return false; }
  return true;
}


void
RemSpeciesItem::redo(Ast::Model &model)
{
  Ast::Species *species = model.getSpecies(_var_id);
  model.remDefinition(species);
}

void
RemSpeciesItem::undo(Ast::Model &model)
{
  // Get referenced compartment
  Ast::Compartment *compartment = model.getCompartment(_compartment);
  // Parse expression in global context
  GiNaC::ex value = Parser::Expr::parseExpression(_value, &model);
  // Create species
  Ast::Species *species = new Ast::Species(_var_id, value, compartment, _name, _is_constant);
  // Add to model
  model.addDefinition(species);
}




/* ********************************************************************************************* *
 * Implementation add compartment item
 * ********************************************************************************************* */
AddCompartmentItem::AddCompartmentItem(Ast::Compartment *compartment, const Ast::Model &parent)
  : FullVariableItem(compartment, parent), _dimension(compartment->getDimension())
{
  // Pass...
}

AddCompartmentItem::~AddCompartmentItem()
{
  // Pass...
}


bool
AddCompartmentItem::canUndo(const Ast::Model &model)
{
  // A compartment can be removed if it exists
  if (! model.hasCompartment(_var_id)) { return false; }
  Ast::Compartment *compartment = model.getCompartment(_var_id);
  // and is not referenced
  if (0 != ReferenceCounter::count(compartment, model)) { return false; }
  return true;
}

bool
AddCompartmentItem::canRedo(const Ast::Model &model) {
  // A compartment can be created if there is no definition with the same identifier:
  if (model.hasDefinition(_var_id)) { return false; }
  return true;
}


void
AddCompartmentItem::undo(Ast::Model &model)
{
  Ast::Compartment *compartment = model.getCompartment(_var_id);
  model.remDefinition(compartment);
}

void
AddCompartmentItem::redo(Ast::Model &model)
{
  // Parse expression in global context
  GiNaC::ex value = Parser::Expr::parseExpression(_value, &model);
  // Create Compartment
  Ast::Compartment *compartment = new Ast::Compartment(_var_id, value, _dimension, _is_constant);
  // Add to model
  model.addDefinition(compartment);
}




/* ********************************************************************************************* *
 * Implementation rem compartment item
 * ********************************************************************************************* */
RemCompartmentItem::RemCompartmentItem(Ast::Compartment *compartment, const Ast::Model &parent)
  : FullVariableItem(compartment, parent), _dimension(compartment->getDimension())
{
  // Pass...
}

RemCompartmentItem::~RemCompartmentItem()
{
  // Pass...
}


bool
RemCompartmentItem::canRedo(const Ast::Model &model)
{
  // A compartment can be removed if it exists
  if (! model.hasCompartment(_var_id)) { return false; }
  Ast::Compartment *compartment = model.getCompartment(_var_id);
  // and is not referenced
  if (0 != ReferenceCounter::count(compartment, model)) { return false; }
  return true;
}

bool
RemCompartmentItem::canUndo(const Ast::Model &model) {
  // A compartment can be created if there is no definition with the same identifier:
  if (model.hasDefinition(_var_id)) { return false; }
  return true;
}


void
RemCompartmentItem::redo(Ast::Model &model)
{
  Ast::Compartment *compartment = model.getCompartment(_var_id);
  model.remDefinition(compartment);
}

void
RemCompartmentItem::undo(Ast::Model &model)
{
  // Parse expression in global context
  GiNaC::ex value = Parser::Expr::parseExpression(_value, &model);
  // Create Compartment
  Ast::Compartment *compartment = new Ast::Compartment(_var_id, value, _dimension, _is_constant);
  // Add to model
  model.addDefinition(compartment);
}




/* ********************************************************************************************* *
 * Implementation add parameter item
 * ********************************************************************************************* */
AddParameterItem::AddParameterItem(Ast::Parameter *param, const Ast::Model &model)
  : FullVariableItem(param, model), _unit(param->getUnit())
{
  // Pass...
}

AddParameterItem::AddParameterItem(Ast::Parameter *param, const Ast::Reaction *reaction)
  : FullVariableItem(param, reaction), _unit(param->getUnit())
{
  // Pass...
}

AddParameterItem::~AddParameterItem()
{
  // Pass...
}


bool
AddParameterItem::canUndo(const Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Check if parent exists:
    if (! model.hasReaction(_parent_id)) { return false; }
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // Check if param exists:
    if (! reaction->getKineticLaw()->hasVariable(_var_id)) { return false; }
    Ast::Parameter *param = reaction->getKineticLaw()->getParameter(_var_id);
    // Check if param is referenced:
    if (0 != ReferenceCounter::count(param, model)) { return false; }
  } else {
    // A parameter can be removed if it exists
    if (! model.hasParameter(_var_id)) { return false; }
    const Ast::Parameter *param = model.getParameter(_var_id);
    // check if param is referenced
    if (0 != ReferenceCounter::count(param, model)) { return false; }
  }
  return true;
}


bool
AddParameterItem::canRedo(const Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Check if parent exists:
    if (! model.hasReaction(_parent_id)) { return false; }
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // check if param is defined:
    if (reaction->getKineticLaw()->hasDefinition(_var_id, true)) { return false; }
  } else {
    // Check if param is defined:
    if (model.hasDefinition(_var_id)) { return false; }
  }
  return true;
}



void
AddParameterItem::undo(Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Get parent
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // Get parameter
    Ast::Parameter *param = reaction->getKineticLaw()->getParameter(_var_id);
    // delete parameter:
    reaction->getKineticLaw()->remDefinition(param);
  } else {
    // Get Parameter
    Ast::Parameter *param = model.getParameter(_var_id);
    // remove
    model.remDefinition(param);
  }
}


void
AddParameterItem::redo(Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Get parent
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // Parse initial value:
    GiNaC::ex value = Parser::Expr::parseExpression(_value, reaction->getKineticLaw());
    // Create parameter:
    Ast::Parameter *param = new Ast::Parameter(_var_id, value, _unit, _is_constant);
    // add parameter
    reaction->getKineticLaw()->addDefinition(param);
  } else {
    // Parse initial value:
    GiNaC::ex value = Parser::Expr::parseExpression(_value, &model);
    // Create parameter:
    Ast::Parameter *param = new Ast::Parameter(_var_id, value, _unit, _is_constant);
    // add parameter
    model.addDefinition(param);
  }
}




/* ********************************************************************************************* *
 * Implementation rem parameter item
 * ********************************************************************************************* */
RemParameterItem::RemParameterItem(Ast::Parameter *param, const Ast::Model &model)
  : FullVariableItem(param, model), _unit(param->getUnit())
{
  // Pass...
}

RemParameterItem::RemParameterItem(Ast::Parameter *param, const Ast::Reaction *reaction)
  : FullVariableItem(param, reaction), _unit(param->getUnit())
{
  // Pass...
}

RemParameterItem::~RemParameterItem()
{
  // Pass...
}


bool
RemParameterItem::canRedo(const Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Check if parent exists:
    if (! model.hasReaction(_parent_id)) { return false; }
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // Check if param exists:
    if (! reaction->getKineticLaw()->hasVariable(_var_id)) { return false; }
    Ast::Parameter *param = reaction->getKineticLaw()->getParameter(_var_id);
    // Check if param is referenced:
    if (0 != ReferenceCounter::count(param, model)) { return false; }
  } else {
    // A parameter can be removed if it exists
    if (! model.hasParameter(_var_id)) { return false; }
    const Ast::Parameter *param = model.getParameter(_var_id);
    // check if param is referenced
    if (0 != ReferenceCounter::count(param, model)) { return false; }
  }
  return true;
}


bool
RemParameterItem::canUndo(const Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Check if parent exists:
    if (! model.hasReaction(_parent_id)) { return false; }
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // check if param is defined:
    if (reaction->getKineticLaw()->hasDefinition(_var_id, true)) { return false; }
  } else {
    // Check if param is defined:
    if (model.hasDefinition(_var_id)) { return false; }
  }
  return true;
}



void
RemParameterItem::redo(Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Get parent
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // Get parameter
    Ast::Parameter *param = reaction->getKineticLaw()->getParameter(_var_id);
    // delete parameter:
    reaction->getKineticLaw()->remDefinition(param);
  } else {
    // Get Parameter
    Ast::Parameter *param = model.getParameter(_var_id);
    // remove
    model.remDefinition(param);
  }
}


void
RemParameterItem::undo(Ast::Model &model)
{
  // Switch if param is local
  if (0 != _parent_id.size()) {
    // Get parent
    Ast::Reaction *reaction = model.getReaction(_parent_id);
    // Parse initial value:
    GiNaC::ex value = Parser::Expr::parseExpression(_value, reaction->getKineticLaw());
    // Create parameter:
    Ast::Parameter *param = new Ast::Parameter(_var_id, value, _unit, _is_constant);
    // add parameter
    reaction->getKineticLaw()->addDefinition(param);
  } else {
    // Parse initial value:
    GiNaC::ex value = Parser::Expr::parseExpression(_value, &model);
    // Create parameter:
    Ast::Parameter *param = new Ast::Parameter(_var_id, value, _unit, _is_constant);
    // add parameter
    model.addDefinition(param);
  }
}
