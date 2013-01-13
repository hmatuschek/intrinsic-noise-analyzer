#ifndef __INA_APP_MODELS_SCOPEITEMMODEL_HH__
#define __INA_APP_MODELS_SCOPEITEMMODEL_HH__

#include <QAbstractListModel>
#include "ast/scope.hh"
#include <QVector>
#include <QString>
#include <QPair>



/** This simple @c QAbstractListModel implements a list of variable names/symbols defined in a
 * scope and its parent scopes. This model is used for the auto-completion of expressions. */
class ScopeItemModel : public QAbstractListModel
{
  Q_OBJECT

public:
  /** Filter flags. */
  typedef enum {
    /** Nothing will be collected. */
    SELECT_NONE    = 0,
    /** Collect only species. */
    SELECT_SPECIES = 1,
    /** Collect only compartments. */
    SELECT_COMPARTMENTS = 2,
    /** Collect only paramters. */
    SELECT_PARAMETERS = 4,
    /** Collect all. */
    SELECT_ALL = SELECT_SPECIES | SELECT_COMPARTMENTS | SELECT_PARAMETERS
  } TypeSelection;

public:
  /** Constructor. */
  explicit ScopeItemModel(const iNA::Ast::Scope &scope, QObject *parent = 0);

  /** Returns the number of items in the list. */
  int rowCount(const QModelIndex &parent=QModelIndex()) const;
  /** Returns the name or identifier of the items. */
  QVariant data(const QModelIndex &index, int role) const;
  /** Retunrs a header if needed. */
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


#endif // __INA_APP_MODELS_SCOPEITEMMODEL_HH__
