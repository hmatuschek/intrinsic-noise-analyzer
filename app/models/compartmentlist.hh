#ifndef __INA_APP_MODELS_COMPARTMENTLIST_HH__
#define __INA_APP_MODELS_COMPARTMENTLIST_HH__

#include <QAbstractTableModel>
#include "ast/model.hh"


/** Implements a @c QAbstractTableModel and represents the compartments of an @c Fluc::Ast::Model
 * instance. */
class CompartmentList : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructs a @c QAbstractTableModel holding the compartments of the given SBML model and
   * their attributes. */
  explicit CompartmentList(Fluc::Ast::Model *_model, QObject *parent = 0);

  /** Enables the columns of the compartment names and initial value to be editable. */
  Qt::ItemFlags flags(const QModelIndex &index) const;

  /** Implementation for the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Resets the compartment name or initial value. */
  bool setData(const QModelIndex &index, const QVariant &value, int role);

  /** Implementation of QAbstractTableModel, returns the headers for columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of compartments. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns a weak reference to the Ast::Model. */
  Fluc::Ast::Model &model();


public slots:
  /** Adds a new compartment to the model. */
  void addCompartment();

  /** Removes the compartment from the model. */
  void remCompartment(int row);


private:
  /** Returns the identifier. */
  QVariant _getIdentifier(Fluc::Ast::Compartment *compartment, int role) const;

  /** Returns the name of the compartment. */
  QVariant _getName(Fluc::Ast::Compartment *compartment, int role) const;
  /** Updates the name of the compartment. */
  bool _updateName(Fluc::Ast::Compartment *compartment, const QVariant &value);

  /** Returns the initial value of the compartment. */
  QVariant _getInitValue(Fluc::Ast::Compartment *compartment, int role) const;
  /** Updates the initial value of the compartment. */
  bool _updateInitValue(Fluc::Ast::Compartment *compartment, const QVariant &value);

  /** Returns the unit of the compartment. */
  QVariant _getUnit(Fluc::Ast::Compartment *compartment, int role) const;

  /** Returns the const flag of the compartment. */
  QVariant _getConstFlag(Fluc::Ast::Compartment *compartment, int role) const;
  /** Updates the const flag of the compartment. */
  bool _updateConstFlag(Fluc::Ast::Compartment *compartment, const QVariant &value);


private:
  /** Holds a weak reference to the SBML model. */
  Fluc::Ast::Model *_model;
};


#endif // COMPARTMENTLIST_HH
