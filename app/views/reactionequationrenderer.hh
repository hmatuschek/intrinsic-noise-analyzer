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
};

#endif // REACTIONEQUATIONRENDERER_HH
