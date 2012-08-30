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
  explicit ReactionList(Fluc::Ast::Model *model, QObject *parent = 0);

  /** Returns a weak reference to the model. */
  Fluc::Ast::Model &getModel();

  /** Implementation for the QAbstractTableModel. Returns the requested data. */
  QVariant data(const QModelIndex &index, int role) const;

  /** Implementation of QAbstractItemMode, returns the headers for columns. */
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;

  /** Returns the number of parameters. */
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  /** Returns the number of columns. */
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
  QVariant getName(Fluc::Ast::Reaction *reaction, int role) const;
  QVariant getReaction(Fluc::Ast::Reaction *reaction, int role) const;
  QVariant getKineticLaw(Fluc::Ast::Reaction *reaction, int role) const;

private:
  Fluc::Ast::Model *_model;

};

#endif // __INA_APP_MODELS_REACTIONLIST_HH__
