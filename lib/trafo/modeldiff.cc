#include "modeldiff.hh"
#include <ast/reaction.hh>
#include <parser/expr/parser.hh>


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
ModelDiffVariableItem::ModelDiffVariableItem(const std::string &id)
  : ModelDiffItem(), _var_id(id), _parent_id("")
{
  // Pass...
}


ModelDiffVariableItem::ModelDiffVariableItem(const std::string &id, const std::string &parent_id)
  : ModelDiffItem(), _var_id(id), _parent_id(parent_id)
{
  // Pass...
}


ModelDiffVariableItem::~ModelDiffVariableItem()
{
  // pass...
}


bool
ModelDiffVariableItem::canUndo(const Ast::Model &model)
{
  // can be undone if the referenced variable exists:
  if (0 == _parent_id.size())
    return _hasVariable(_var_id, model);
  return _hasVariable(_var_id, _parent_id, model);
}

bool
ModelDiffVariableItem::canRedo(const Ast::Model &model)
{
  // can be redone if the referenced variable exists:
  if (0 == _parent_id.size())
    return _hasVariable(_var_id, model);
  return _hasVariable(_var_id, _parent_id, model);
}


bool
ModelDiffVariableItem::_hasVariable(const std::string &id, const Ast::Model &model)
{
  return model.hasVariable(id);
}

bool
ModelDiffVariableItem::_hasVariable(const std::string &id, const std::string &parent,
                                    const Ast::Model &model)
{
  if (! model.hasReaction(parent)) { return false; }
  Ast::Reaction *reac = model.getReaction(parent);
  return reac->getKineticLaw()->hasVariable(id);
}


Ast::VariableDefinition *
ModelDiffVariableItem::_getVariable(const std::string &id, const Ast::Model &model)
{
  return model.getVariable(id);
}

Ast::VariableDefinition *
ModelDiffVariableItem::_getVariable(const std::string &id, const std::string &parent, const Ast::Model &model)
{
  Ast::Reaction *reac = model.getReaction(parent);
  return reac->getKineticLaw()->getVariable(id);
}



/* ********************************************************************************************* *
 * Implementation set variable identifier modification
 * ********************************************************************************************* */
SetVariableIdentifierItem::SetVariableIdentifierItem(const std::string &id, const std::string &new_id)
  : ModelDiffVariableItem(id), _new_identifier(new_id)
{
  // Pass...
}

SetVariableIdentifierItem::SetVariableIdentifierItem(const std::string &id, const std::string &new_id,
                                                     const std::string &parent_id)
  : ModelDiffVariableItem(id, parent_id), _new_identifier(new_id)
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
  : ModelDiffVariableItem(id), _old_name(old_name), _new_name(new_name)
{
  // pass...
}

SetVariableNameItem::SetVariableNameItem(const std::string &id, const std::string &parent_id,
                                         const std::string &old_name, const std::string &new_name)
  : ModelDiffVariableItem(id, parent_id), _old_name(old_name), _new_name(new_name)
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
  return ModelDiffVariableItem::canUndo(model);
}

bool
SetVariableNameItem::canRedo(const Ast::Model &model)
{
  return ModelDiffVariableItem::canRedo(model);
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
  : ModelDiffVariableItem(id), _old_state(old_state), _new_state(new_state)
{
  // pass...
}

SetVariableConstFlagItem::SetVariableConstFlagItem(const std::string &id, const std::string &parent_id,
                                                   bool old_state, bool new_state)
  : ModelDiffVariableItem(id, parent_id), _old_state(old_state), _new_state(new_state)
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
  return ModelDiffVariableItem::canUndo(model);
}

bool
SetVariableConstFlagItem::canRedo(const Ast::Model &model)
{
  return ModelDiffVariableItem::canRedo(model);
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
  : ModelDiffVariableItem(id), _old_value(old_value), _new_value(new_value)
{
  // pass...
}

SetVariableValueItem::SetVariableValueItem(const std::string &id, const std::string &parent_id,
                                           const std::string &old_value, const std::string &new_value)
  : ModelDiffVariableItem(id, parent_id), _old_value(old_value), _new_value(new_value)
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
  if (! ModelDiffVariableItem::canUndo(model)) { return false; }
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
  if (! ModelDiffVariableItem::canUndo(model)) { return false; }
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
