#ifndef REACTIONEQUATIONRENDERER_HH
#define REACTIONEQUATIONRENDERER_HH

#include <QGraphicsScene>
#include "ast/scope.hh"
#include "ast/species.hh"
#include "../tinytex/custommula.hh"

/** Implements a simple reaction-equation renderer using tinyTeX. */
class ReactionEquationRenderer : public QGraphicsScene
{
  Q_OBJECT

public:
  /** Constructs an empty rendered reaction. */
  explicit ReactionEquationRenderer(iNA::Ast::Reaction *reaction, QObject *parent = 0);

public:
  /** Directly renders the reaction equation into a pixmap. */
  static QPixmap renderReaction(
    iNA::Ast::Reaction *reaction, MathContext ctx=MathContext());
  /** Directly renders the reaction equation into a pixmap. */
  static QPixmap renderReactionEquation(
    iNA::Ast::Reaction *reaction, MathContext ctx=MathContext());
  /** Directly renders the reaction equation into a pixmap. */
  static QPixmap renderKineticLaw(
    iNA::Ast::Reaction *reaction, MathContext ctx=MathContext());

protected:
  /** Assembles only the reaction equation. */
  static MathItem *assembleReactionEquation(iNA::Ast::Reaction *reaction);
  /** Assembles only the kinetic law expression. */
  static MathItem *assembleKineticLaw(iNA::Ast::Reaction *reaction);
  /** Assembles the complete reaction (equation + kinetic law). */
  static MathItem *assembleReaction(iNA::Ast::Reaction *reaction);
};

#endif // REACTIONEQUATIONRENDERER_HH
