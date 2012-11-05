#ifndef __INA_APP_MODELS_REACTIONLIST_HH__
#define __INA_APP_MODELS_REACTIONLIST_HH__

#include <QAbstractTableModel>

#include <ast/model.hh>
#include <ast/reaction.hh>


/** Represents all reactions of a @c Ast::Model as a list. */
class ReactionList : public QAbstractTableModel
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ReactionList(iNA::Ast::Model *model, QObject *parent = 0);

  /** Returns a weak reference to the model. */
  iNA::Ast::Model &getModel();

  /** Implementation custom the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Implementation of QAbstractItemMode, returns the headers custom columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of parameters. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  /** A updates the complete table. */
  void updateTable();


private:
  QVariant getName(iNA::Ast::Reaction *reaction, int role) const;
  QVariant getReaction(iNA::Ast::Reaction *reaction, int role) const;
  QVariant getKineticLaw(iNA::Ast::Reaction *reaction, int role) const;

private:
  iNA::Ast::Model *_model;

};

#endif // __INA_APP_MODELS_REACTIONLIST_HH__
