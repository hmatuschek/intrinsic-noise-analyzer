#ifndef __INA_APP_DOCTREE_REACTIONITEM_HH__
#define __INA_APP_DOCTREE_REACTIONITEM_HH__

#include <QObject>
#include "documenttreeitem.hh"
#include "../models/reactionparameterlist.hh"

/**
 * Represents a single @c Ast::Reaction for the display in the application side-panel.
 *
 * This wrapper also provides the construction of a @c ReactionView instance for the reaction.
 *
 * @ingroup gui
 */
class ReactionItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

protected:
  /** Holds a weak reference to the reaction. */
  Fluc::Ast::Reaction *_reaction;
  /** Holds a weak reference to the model. */
  Fluc::Ast::Model *_model;
  /** Hold an instance of the local paramter list of the kinetic law. */
  ReactionParameterList *local_parameters;
  /** Holds the display name of the reaction. */
  QString itemLabel;

public:
  explicit ReactionItem(Fluc::Ast::Reaction *_reaction, Fluc::Ast::Model *model,
                        QObject *parent=0);

  virtual const QString &getLabel() const;

  QString getDisplayName() const;

  Fluc::Ast::Reaction *getReaction();

  ReactionParameterList *localParameters();

  virtual bool providesView() const;

  virtual QWidget *createView();
};


#endif // __INA_APP_DOCTREE_REACTIONITEM_HH__
