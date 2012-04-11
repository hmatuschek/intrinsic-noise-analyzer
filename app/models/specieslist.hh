#ifndef __INA_APP_MODELS_SPECIESLIST_HH__
#define __INA_APP_MODELS_SPECIESLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"


class SpeciesList : public QAbstractTableModel
{
  Q_OBJECT


public:
  explicit SpeciesList(Fluc::Ast::Model *model, QObject *parent = 0);

  /**
   * Implementation for the QAbstractTableModel. Returns the requested data.
   */
  QVariant data(const QModelIndex &index, int role) const;

  /**
   * Implementation of QAbstractItemMode, returns the headers for columns.
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /**
   * Returns the number of parameters.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /**
   * Returns the number of columns.
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;


private:
  /**
   * Internal used function to assemble a string representation of the initial value of a species.
   */
  QString getInitialValueForSpecies(Fluc::Ast::Species *species) const;


private:
  /**
   * Holds a weak reference to the SBML model.
   */
  Fluc::Ast::Model *model;
};


#endif // SPECIESLIST_HH
