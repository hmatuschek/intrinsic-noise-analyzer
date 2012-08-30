#include "reactionequationrenderer.hh"
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include "../tinytex/tinytex.hh"
#include "../tinytex/ginac2formula.hh"
#include "ast/reaction.hh"


using namespace Fluc;

ReactionEquationRenderer::ReactionEquationRenderer(Fluc::Ast::Reaction *reac, QObject *parent) :
  QGraphicsScene(parent)
{
  // Assemble forumla
  MathFormulaItem *reaction = ReactionEquationRenderer::assembleReaction(reac);

  // layout equation and add to this graphics scene...
  MathContext ctx; ctx.setFontSize(ctx.fontSize()+4);
  this->addItem(reaction->layout(ctx));
  // Free formula
  delete reaction;
}


MathFormulaItem *
ReactionEquationRenderer::assembleReactionEquation(Fluc::Ast::Reaction *reac)
{
  // Allocate formula
  Ast::Scope &scope = *(reac->getKineticLaw());
  MathFormula *reaction  = new MathFormula();
  MathFormula *reactants = new MathFormula();
  MathFormula *products  = new MathFormula();

  // Handle reactants:
  if (0 == reac->numReactants()) { reactants->appendItem(new MathText(QChar(0x2205))); }
  for (Ast::Reaction::iterator item=reac->reacBegin(); item!=reac->reacEnd(); item++) {
    if (0 != reactants->size()) {
      reactants->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
      reactants->appendItem(new MathText("+"));
      reactants->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    }
    if (1 != item->second) {
      reactants->appendItem(Ginac2Formula::toFormula(item->second, scope));
      reactants->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    }
    // Render species symbol
    reactants->appendItem(TinyTex::parseVariable(item->first));
  }
  reaction->appendItem(reactants);

  // " -> "
  reaction->appendItem(new MathSpace(MathSpace::THICK_SPACE));
  if (reac->isReversible()) {
    reaction->appendItem(new MathText(QChar(0x21CC)));
  } else {
    reaction->appendItem(new MathText(QChar(0x2192)));
  }
  reaction->appendItem(new MathSpace(MathSpace::THICK_SPACE));

  // handle products
  if (0 == reac->numProducts()) { products->appendItem(new MathText(QChar(0x2205))); }
  for (Ast::Reaction::iterator item=reac->prodBegin(); item!=reac->prodEnd(); item++) {
    // Prepent "+" sign
    if (0 != products->size()) {
      products->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
      products->appendItem(new MathText("+"));
      products->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
    }
    // Prepren stoichiometry if != 1:
    if (1 != item->second) {
      products->appendItem(Ginac2Formula::toFormula(item->second, scope));
      products->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    }
    // render species symbol.
    products->appendItem(TinyTex::parseVariable(item->first));
  }
  reaction->appendItem(products);

  return reaction;
}


MathFormulaItem *
ReactionEquationRenderer::assembleKineticLaw(Fluc::Ast::Reaction *reaction)
{
  // Handle rate law
  Ast::Scope &scope = *(reaction->getKineticLaw());
  return Ginac2Formula::toFormula(reaction->getKineticLaw()->getRateLaw(), scope);
}


MathFormulaItem *
ReactionEquationRenderer::assembleReaction(Fluc::Ast::Reaction *reac)
{
  MathFormula *reaction = new MathFormula();

  reaction->appendItem(ReactionEquationRenderer::assembleReactionEquation(reac));
  reaction->appendItem(new MathSpace(MathSpace::QUAD_SPACE));
  reaction->appendItem(new MathText(":"));
  reaction->appendItem(new MathSpace(MathSpace::MEDIUM_SPACE));
  reaction->appendItem(ReactionEquationRenderer::assembleKineticLaw(reac));

  return reaction;
}



QPixmap
ReactionEquationRenderer::renderReaction(Fluc::Ast::Reaction *reaction, MathContext ctx)
{
  MathFormulaItem *formula = ReactionEquationRenderer::assembleReaction(reaction);
  QPixmap pixmap = formula->renderItem(ctx); delete formula;
  return pixmap;
}

QPixmap
ReactionEquationRenderer::renderReactionEquation(Fluc::Ast::Reaction *reaction, MathContext ctx)
{
  MathFormulaItem *formula = ReactionEquationRenderer::assembleReactionEquation(reaction);
  QPixmap pixmap = formula->renderItem(ctx); delete formula;
  return pixmap;
}

QPixmap
ReactionEquationRenderer::renderKineticLaw(Fluc::Ast::Reaction *reaction, MathContext ctx)
{
  MathFormulaItem *formula = ReactionEquationRenderer::assembleKineticLaw(reaction);
  QPixmap pixmap = formula->renderItem(ctx); delete formula;
  return pixmap;
}
