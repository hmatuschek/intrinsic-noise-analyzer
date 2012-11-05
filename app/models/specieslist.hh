#ifndef __INA_APP_MODELS_SPECIESLIST_HH__
#define __INA_APP_MODELS_SPECIESLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"

/** This table model manages the list of defined species and implements the major part that
 * allows to edit these species. */
class SpeciesList : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SpeciesList(iNA::Ast::Model *_model, QObject *parent = 0);

  /** Signals, if a cell is editable. */
  Qt::ItemFlags flags(const QModelIndex &index) const;

  /** Implementation custom the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Implements the editing of names and initial values. */
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  /** Implementation of QAbstractItemMode, returns the headers custom columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of species. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the underlying @c Ast::Model */
  iNA::Ast::Model &model();

public slots:
  /** Adds a dummy species to the model. */
  void addSpecies();
  /** Tries to remove a species from the model. */
  void remSpecies(int row);

private:
  /** Returns the identifier. */
  QVariant _getIdentifier(iNA::Ast::Species *species, int role) const;
  /** Updates the identifier of the species. */
  bool _updateIdentifier(iNA::Ast::Species *species, const QVariant &value);

  /** Iternal function to get the species name. */
  QVariant _getName(iNA::Ast::Species *species, int role) const;
  /** Internal function to update the species name. */
  bool _updateName(iNA::Ast::Species *species, const QVariant &value);

  /** Internal function to get the initial value of the species. */
  QVariant _getInitialValue(iNA::Ast::Species *species, int role) const;
  /** Internal function to update the initial value of the species. */
  bool _updateInitialValue(iNA::Ast::Species *species, const QVariant &value);

  /** Internal function to get the unit. */
  QVariant _getUnit(iNA::Ast::Species *species, int role) const;
  /** Updates the unit of the species. */
  bool _updateUnit(iNA::Ast::Species *species, const QVariant &value);

  /** Internal function to get const flag. */
  QVariant _getConstFlag(iNA::Ast::Species *species, int role) const;
  /** Internal function to update the constant flag of the species. */
  bool _updateConstFlag(iNA::Ast::Species *species, const QVariant &value);

  /** Internal function to get species compartment. */
  QVariant _getCompartment(iNA::Ast::Species *species, int role) const;
  /** Internal function to update the compartment of the species. */
  bool _updateCompartment(iNA::Ast::Species *species, const QVariant &value);

  /** Internal function to get species rule. */
  QVariant _getRule(iNA::Ast::Species *species, int role) const;
  /** Internal function to update the rule of the species. */
  bool _updateRule(iNA::Ast::Species *species, const QVariant &value);

private:
  /** Holds a weak reference to the SBML model. */
  iNA::Ast::Model *_model;
};


#endif // SPECIESLIST_HH
