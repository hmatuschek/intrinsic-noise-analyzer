#include "reactionequationrenderer.hh"
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include "../tinytex/ginac2formula.hh"
#include "ast/reaction.hh"


using namespace Fluc;

ReactionEquationRenderer::ReactionEquationRenderer(Fluc::Ast::Reaction *reac, QObject *parent) :
  QGraphicsScene(parent)
{
  Ast::Scope &scope = *(reac->getKineticLaw());
  MathFormula reaction;
  MathFormula *reactants = new MathFormula();
  MathFormula *products = new MathFormula();

  // Handle reactants:
  for (Ast::Reaction::iterator item=reac->reacBegin(); item!=reac->reacEnd(); item++) {
    if (0 != reactants->size()) {
      reactants->appendItem(new MathText(" + "));
    }
    if (1 != item->second) {
      reactants->appendItem(Ginac2Formula::toFormula(item->second, scope));
    }
    reactants->appendItem(Ginac2Formula::toFormula(item->first->getSymbol(), scope));
  }
  reaction.appendItem(reactants);

  // ->
  reaction.appendItem(new MathText(QChar(0x21C0)));

  // handle products
  for (Ast::Reaction::iterator item=reac->prodBegin(); item!=reac->prodEnd(); item++) {
    if (0 != products->size()) {
      products->appendItem(new MathText(" + "));
    }
    if (1 != item->second) {
      products->appendItem(Ginac2Formula::toFormula(item->second, scope));
    }
    products->appendItem(Ginac2Formula::toFormula(item->first->getSymbol(), scope));
  }
  reaction.appendItem(products);

  reaction.appendItem(new MathText(" : "));

  // Handle rate law
  reaction.appendItem(Ginac2Formula::toFormula(reac->getKineticLaw()->getRateLaw(), scope));

  // layout equation and add to this graphics scene...
  MathContext ctx; ctx.setFontSize(ctx.fontSize()+4);
  this->addItem(reaction.layout(ctx));
}
