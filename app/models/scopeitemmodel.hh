#ifndef __INA_APP_MODELS_SCOPEITEMMODEL_HH__
#define __INA_APP_MODELS_SCOPEITEMMODEL_HH__

#include <QAbstractListModel>
#include "ast/scope.hh"
#include <QVector>
#include <QString>
#include <QPair>



/** This simple @c QAbstractListModel implements a list of variable names/symbols defined is a
 * scope and its parent scopes. */
class ScopeItemModel : public QAbstractListModel
{
  Q_OBJECT

public:
  explicit ScopeItemModel(const Fluc::Ast::Scope &scope, QObject *parent = 0);

  int rowCount(const QModelIndex &parent) const;

  QVariant data(const QModelIndex &index, int role) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;


private:
  /** Holds a vector <name, id> of each variable defined in the scope. */
  QVector< QPair<QString, QString> > _items;
};


#endif // SCOPEITEMMODEL_HH
