#ifndef __INA_APP_MODELS_PARAMETERLIST_HH__
#define __INA_APP_MODELS_PARAMETERLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"


/** This class implements the @c QAbstractTableModel, providing an model for a @c QTableView
 * widget. The table model lists all paramters of a @c Ast::Model and allows to edit them as
 * well as adding and removing parameters. */
class ParameterList : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructs a parameter list from the given SBML model. */
  explicit ParameterList(iNA::Ast::Model *_model, QObject *parent = 0);

  /** Implementation for the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Implements setting the parameter initial value or name. */
  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

  /** Returns a flag, that indicates if a cell is editable. */
  Qt::ItemFlags flags(const QModelIndex &index) const;

  /** Implementation of QAbstractItemMode, returns the headers for columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of parameters. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns a weak reference to the model instance. */
  iNA::Ast::Model &model();

  /** Adds a new paramter to the list of paramters. */
  void addParameter();

  /** Removes a parameter if it is not in use. */
  void remParameter(int row);

signals:
  /** Gets emitted once the model gets modified. */
  void modelModified();

private:
  /** Retunrs the identifier of the parameter. */
  QVariant _getIdentifier(iNA::Ast::Parameter *param, int role) const;
  /** Updates the identifier of the paramter. */
  bool _updateIdentifier(iNA::Ast::Parameter *param, const QVariant &value);
  /** Retuns the name of the paramter. */
  QVariant _getName(iNA::Ast::Parameter *param, int role) const;
  /** Retuns the name of the paramter. */
  bool _updateName(iNA::Ast::Parameter *param, const QVariant &value);
  /** Returns the intial value of the paramter. */
  QVariant _getInitialValue(iNA::Ast::Parameter *param, int role) const;
  /** Updates the initial value of the paramter. */
  bool _updateInitialValue(iNA::Ast::Parameter *param, const QVariant &value);
  /** Return the unit of the parameter. */
  QVariant _getUnit(iNA::Ast::Parameter *param, int role) const;
  /** Updates the parameter unit. */
  bool _updateUnit(iNA::Ast::Parameter *param, const QVariant &value);
  /** Returns the const flag. */
  QVariant _getConstFlag(iNA::Ast::Parameter *param, int role) const;
  /** Updates the const flag. */
  bool _updateConstFlag(iNA::Ast::Parameter *param, const QVariant &value);

private:
  /** Holds a weak reference to the SBML model. */
  iNA::Ast::Model *_model;
};

#endif // PARAMETERLIST_HH
