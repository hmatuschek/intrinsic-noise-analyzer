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
  typedef enum {
    SELECT_NONE    = 0,
    SELECT_SPECIES = 1,
    SELECT_COMPARTMENTS = 2,
    SELECT_PARAMETERS = 4,
    SELECT_ALL = SELECT_SPECIES | SELECT_COMPARTMENTS | SELECT_PARAMETERS
  } TypeSelection;

public:
  explicit ScopeItemModel(const iNA::Ast::Scope &scope, QObject *parent = 0);

  int rowCount(const QModelIndex &parent=QModelIndex()) const;

  QVariant data(const QModelIndex &index, int role) const;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const;


public:
  /** Assembles a list of identifiers of all variables defined in the given scope that match the
   * given type selection. */
  static QStringList &collectIdentifiers(QStringList &ids, const iNA::Ast::Scope &scope, unsigned selection);

  /** Assembles a list of identifiers of all variables defined in the given scope that match the
   * given type selection. */
  static QStringList collectIdentifiers(const iNA::Ast::Scope &scope, unsigned selection);

private:
  /** Holds a vector <name, id> of each variable defined in the scope. */
  QVector< QPair<QString, QString> > _items;

};


#endif // SCOPEITEMMODEL_HH
