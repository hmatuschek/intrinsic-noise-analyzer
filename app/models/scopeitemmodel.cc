#include "scopeitemmodel.hh"
#include <QStringList>

using namespace iNA;


ScopeItemModel::ScopeItemModel(const iNA::Ast::Scope &scope, QObject *parent)
  : QAbstractListModel(parent), _items()
{
  std::set<std::string> ids;
  const Ast::Scope *current_scope = &scope;
  while(0 != current_scope) {
    for (Ast::Scope::const_iterator item=current_scope->begin(); item!=current_scope->end(); item++) {
      if (! Ast::Node::isVariableDefinition(*item)) { continue; }
      Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*item);
      QString id = var->getIdentifier().c_str();
      QString name = id;
      if (var->hasName()) { name = QString("%1 (%2)").arg(var->getName().c_str(), id); }
      if (ids.end() == ids.find(var->getIdentifier())) {
        ids.insert(var->getIdentifier());
        _items.push_back(QPair<QString, QString>(name, id));
      }
    }

    // Traverse into parent scopes
    if (current_scope->hasParentScope()) { current_scope = current_scope->getParentScope(); }
    else { current_scope = 0; }
  }
}


int
ScopeItemModel::rowCount(const QModelIndex &parent) const {
  return _items.size();
}


QVariant
ScopeItemModel::data(const QModelIndex &index, int role) const {
  if ((! index.isValid()) || (index.row() >= rowCount())) { QVariant(); }

  if (Qt::DisplayRole == role) {
    return _items[index.row()].first;
  } else if (Qt::EditRole == role){
    return _items[index.row()].second;
  }
  return QVariant();
}


QVariant
ScopeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}


QStringList &
ScopeItemModel::collectIdentifiers(QStringList &ids, const iNA::Ast::Scope &scope, unsigned selection)
{
  const Ast::Scope *current_scope = &scope;
  while(0 != current_scope) {
    for (Ast::Scope::const_iterator item=current_scope->begin(); item!=current_scope->end(); item++) {
      if (! Ast::Node::isVariableDefinition(*item)) { continue; }
      if (Ast::Node::isSpecies(*item) && !(SELECT_SPECIES & selection)) { continue; }
      if (Ast::Node::isCompartment(*item) && !(SELECT_COMPARTMENTS & selection)) { continue; }
      if (Ast::Node::isParameter(*item) && !(SELECT_PARAMETERS & selection)) { continue; }
      Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*item);
      if (! ids.contains(var->getIdentifier().c_str())) {
        ids.append(var->getIdentifier().c_str());
      }
    }

    if (current_scope->hasParentScope()) { current_scope = current_scope->getParentScope(); }
    else { current_scope = 0; }
  }

  return ids;
}


QStringList
ScopeItemModel::collectIdentifiers(const iNA::Ast::Scope &scope, unsigned selection)
{
  QStringList ids;
  collectIdentifiers(ids, scope, selection);
  return ids;
}
