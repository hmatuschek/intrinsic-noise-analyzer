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
  explicit SpeciesList(Fluc::Ast::Model *_model, QObject *parent = 0);

  /** Signals, if a cell is editable. */
  Qt::ItemFlags flags(const QModelIndex &index) const;

  /** Implementation for the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Implements the editing of names and initial values. */
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  /** Implementation of QAbstractItemMode, returns the headers for columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of species. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the underlying @c Ast::Model */
  Fluc::Ast::Model &model();

public slots:
  /** Adds a dummy species to the model. */
  void addSpecies();
  /** Tries to remove a species from the model. */
  void remSpecies(int row);

private:
  /** Returns the identifier. */
  QVariant _getIdentifier(Fluc::Ast::Species *species, int role) const;

  /** Iternal function to get the species name. */
  QVariant _getName(Fluc::Ast::Species *species, int role) const;
  /** Internal function to update the species name. */
  bool _updateName(Fluc::Ast::Species *species, const QVariant &value);

  /** Internal function to get the initial value of the species. */
  QVariant _getInitialValue(Fluc::Ast::Species *species, int role) const;
  /** Internal function to update the initial value of the species. */
  bool _updateInitialValue(Fluc::Ast::Species *species, const QVariant &value);

  /** Internal function to get the unit. */
  QVariant _getUnit(Fluc::Ast::Species *species, int role) const;
  /** Updates the unit of the species. */
  bool _updateUnit(Fluc::Ast::Species *species, const QVariant &value);

  /** Internal function to get const flag. */
  QVariant _getConstFlag(Fluc::Ast::Species *species, int role) const;
  /** Internal function to update the constant flag of the species. */
  bool _updateConstFlag(Fluc::Ast::Species *species, const QVariant &value);

  /** Internal function to get species compartment. */
  QVariant _getCompartment(Fluc::Ast::Species *species, int role) const;
  /** Internal function to update the compartment of the species. */
  bool _updateCompartment(Fluc::Ast::Species *species, const QVariant &value);

  /** Internal function to get species rule. */
  QVariant _getRule(Fluc::Ast::Species *species, int role) const;
  /** Internal function to update the rule of the species. */
  bool _updateRule(Fluc::Ast::Species *species, const QVariant &value);

private:
  /** Holds a weak reference to the SBML model. */
  Fluc::Ast::Model *_model;
};


#endif // SPECIESLIST_HH
