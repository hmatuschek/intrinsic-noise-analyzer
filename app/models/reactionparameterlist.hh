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
  iNA::Ast::KineticLaw *_kinetic_law;

public:
  /** Constructs a list of local paramters defined in the given @c Fluc::Ast::KineticLaw. */
  explicit ReactionParameterList(iNA::Ast::KineticLaw *law, QObject *parent=0);

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

  /** Returns a weak reference to the kinetic law instance. */
  iNA::Ast::KineticLaw &kineticLaw();

  /** Add a new "empty" parameter to the kinetic law. */
  void addParameter();
  /** Removes the i-th parameter. */
  void remParameter(int idx);
  /** Signals the view that the complete model may have changed. */
  void updateCompleteTable();

private:
  /** Retruns the identifier of the local paramter. */
  QVariant _getIdentifier(iNA::Ast::Parameter *param, int role) const;
  /** Updates the identifier of the local parameter. */
  bool _updateIdentifier(iNA::Ast::Parameter *param, const QVariant &value);
  /** Returns the name of the paramter. */
  QVariant _getName(iNA::Ast::Parameter *param, int role) const;
  /** Updates the name of the parameter. */
  bool _updateName(iNA::Ast::Parameter *param, const QVariant &value);
  /** Returns the initial value of the parameter. */
  QVariant _getInitialValue(iNA::Ast::Parameter *param, int role) const;
  /** Updates the initial value of the paramter. */
  bool _updateInitialValue(iNA::Ast::Parameter *param, const QVariant &value);
  /** Retunrs the unit (rendered). */
  QVariant _getUnit(iNA::Ast::Parameter *param, int role) const;
  /** Returns the const flag of the parameter. Should always be constant. */
  QVariant _getConstFlag(iNA::Ast::Parameter *param, int role) const;
  /** Returns the const flag of the parameter. Should always be constant. */
  bool _updateConstFlag(iNA::Ast::Parameter *param, const QVariant &value);
};



#endif // __INA_APP_MODELS_REACTIONPARAMETERLIST_HH__
