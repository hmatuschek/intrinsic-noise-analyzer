#ifndef __INA_APP_MODELS_PARAMETERLIST_HH__
#define __INA_APP_MODELS_PARAMETERLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"


class ParameterList : public QAbstractTableModel
{
  Q_OBJECT

public:
  /**
   * Constructs a parameter list from the given SBML model.
   */
  explicit ParameterList(Fluc::Ast::Model *model, QObject *parent = 0);

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
   * Returns a string representation of the initial value.
   */
  QString getInitialValueForParameter(Fluc::Ast::Parameter *param) const;


private:
  /**
   * Holds a weak reference to the SBML model.
   */
  Fluc::Ast::Model *model;
};

#endif // PARAMETERLIST_HH
