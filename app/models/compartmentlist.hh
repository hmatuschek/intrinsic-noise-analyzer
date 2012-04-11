#ifndef __INA_APP_MODELS_COMPARTMENTLIST_HH__
#define __INA_APP_MODELS_COMPARTMENTLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"


/**
 * Implements a @c QAbstractTableModel and represents the compartments of an @c Fluc::Ast::Model
 * instance.
 *
 * @ingroup gui
 */
class CompartmentList : public QAbstractTableModel
{
  Q_OBJECT

public:
  /**
   * Constructs a @c QAbstractTableModel holding the compartments of the given SBML model and
   * their attributes.
   */
  explicit CompartmentList(Fluc::Ast::Model *model, QObject *parent = 0);

  /**
   * Implementation for the QAbstractTableModel. Returns the requested data.
   */
  QVariant data(const QModelIndex &index, int role) const;

  /**
   * Implementation of QAbstractTableModel, returns the headers for columns.
   */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /**
   * Returns the number of compartments.
   */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /**
   * Returns the number of columns.
   */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;


private:
  /**
   * Internal used method to serialize initial value.
   */
  QString getInitialValueForCompartment(Fluc::Ast::Compartment *comp) const;


private:
  /**
   * Holds a weak reference to the SBML model.
   */
  Fluc::Ast::Model *model;
};


#endif // COMPARTMENTLIST_HH
