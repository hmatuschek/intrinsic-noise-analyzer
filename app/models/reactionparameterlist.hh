#ifndef __INA_APP_MODELS_REACTIONPARAMETERLIST_HH__
#define __INA_APP_MODELS_REACTIONPARAMETERLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"
#include "ast/reaction.hh"

/**
 * Represents a list of local parameters specific to a reactions KineticLaw.
 *
 * @ingroup gui
 */
class ReactionParameterList : public QAbstractTableModel
{
  Q_OBJECT

protected:
  /** Holds a weak reference to the @c Fluc::Ast::KineticLaw instance. */
  Fluc::Ast::KineticLaw *_kinetic_law;

public:
  /** Constructs a list of local paramters defined in the given @c Fluc::Ast::KineticLaw. */
  explicit ReactionParameterList(Fluc::Ast::KineticLaw *law, QObject *parent=0);

  /** Signals if the given item is editable. */
  Qt::ItemFlags flags(const QModelIndex &index) const;

  /** Implementation for the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Implements resetting the name or value. */
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  /** Implementation of QAbstractItemMode, returns the headers for columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of parameters. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
};



#endif // __INA_APP_MODELS_REACTIONPARAMETERLIST_HH__
