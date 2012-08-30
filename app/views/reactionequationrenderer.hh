#ifndef REACTIONEQUATIONRENDERER_HH
#define REACTIONEQUATIONRENDERER_HH

#include <QGraphicsScene>
#include "ast/scope.hh"
#include "ast/species.hh"
#include "../tinytex/formula.hh"

/** Implements a simple reaction-equation renderer using tinyTeX. */
class ReactionEquationRenderer : public QGraphicsScene
{
  Q_OBJECT

public:
  /** Constructs an empty rendered reaction. */
  explicit ReactionEquationRenderer(Fluc::Ast::Reaction *reaction, QObject *parent = 0);

public:
  /** Directly renders the reaction equation into a pixmap. */
  static QPixmap renderReaction(
    Fluc::Ast::Reaction *reaction, MathContext ctx=MathContext());
  /** Directly renders the reaction equation into a pixmap. */
  static QPixmap renderReactionEquation(
    Fluc::Ast::Reaction *reaction, MathContext ctx=MathContext());
  /** Directly renders the reaction equation into a pixmap. */
  static QPixmap renderKineticLaw(
    Fluc::Ast::Reaction *reaction, MathContext ctx=MathContext());

protected:
  /** Assembles only the reaction equation. */
  static MathFormulaItem *assembleReactionEquation(Fluc::Ast::Reaction *reaction);
  /** Assembles only the kinetic law expression. */
  static MathFormulaItem *assembleKineticLaw(Fluc::Ast::Reaction *reaction);
  /** Assembles the complete reaction (equation + kinetic law). */
  static MathFormulaItem *assembleReaction(Fluc::Ast::Reaction *reaction);
};

#endif // REACTIONEQUATIONRENDERER_HH
