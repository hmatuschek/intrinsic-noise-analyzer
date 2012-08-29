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
  /** Hold an instance of the local paramter list of the kinetic law. */
  ReactionParameterList *_local_parameters;
  /** Holds the display name of the reaction. */
  QString _itemLabel;

public:
  /** Constructor. */
  explicit ReactionItem(Fluc::Ast::Reaction *_reaction, QObject *parent=0);

  /** Retunrs the display name of the reaction. */
  virtual const QString &getLabel() const;

  /** Retunrs the display name of the reaction. */
  QString getDisplayName() const;

  /** Retunrs the reaction instance. */
  Fluc::Ast::Reaction *getReaction();

  /** Returns the list of local paramters. */
  ReactionParameterList *localParameters();

  /** Yes, there is a reaction view. */
  virtual bool providesView() const;

  /** Retrns a new instance of the view. */
  virtual QWidget *createView();
};


#endif // __INA_APP_DOCTREE_REACTIONITEM_HH__
