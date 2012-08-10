#include "scopeitemmodel.hh"

using namespace Fluc;


ScopeItemModel::ScopeItemModel(const Fluc::Ast::Scope &scope, QObject *parent)
  : QAbstractListModel(parent), _items()
{
  const Ast::Scope *current_scope = &scope;
  while(0 != current_scope) {
    for (Ast::Scope::const_iterator item=scope.begin(); item!=scope.end(); item++) {
      if (! Ast::Node::isVariableDefinition(*item)) { continue; }
      Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*item);
      QString name = var->getIdentifier().c_str();
      QString id   = name;
      if (var->hasName()) { name = QString("%1 (%2)").arg(var->getName().c_str(), id); }
      _items.push_back(QPair<QString, QString>(name, id));
    }

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
  if ((! index.isValid()) || index.row() >= rowCount(index)) { QVariant(); }

  if (Qt::DisplayRole == role) {
    return _items[index.row()].first;
  } else {
    return _items[index.row()].second;
  }
}


QVariant
ScopeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  return QVariant();
}
