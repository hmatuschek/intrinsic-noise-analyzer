#ifndef __INA_APP_MODELS_SPECIESLIST_HH__
#define __INA_APP_MODELS_SPECIESLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"


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

private:
  /** Internal used function to assemble a string representation of the initial value of a species. */
  QString getInitialValueForSpecies(Fluc::Ast::Species *species) const;

private:
  /** Holds a weak reference to the SBML model. */
  Fluc::Ast::Model *_model;
};


#endif // SPECIESLIST_HH
