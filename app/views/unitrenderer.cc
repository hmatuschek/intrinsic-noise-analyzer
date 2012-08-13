#include "unitrenderer.hh"
#include <QPaintEngine>


UnitRenderer::UnitRenderer(const Fluc::Ast::Unit &unit, QObject *parent)
  : QObject(parent), _formula(0), _pixmap(0)
{
  _formula = new MathFormula();
  if (1 != unit.getMultiplier()) {
    _formula->appendItem(new MathText(QString("%2").arg(unit.getMultiplier())));
  }
  if (0 != unit.getScale()) {
    MathFormulaItem *item = new MathText(QString("%2").arg(unit.getScale()));
    _formula->appendItem(new MathSup(new MathText("10"), item));
  }

  QList<MathFormulaItem *> nominator;
  QList<MathFormulaItem *> denominator;
  for (Fluc::Ast::Unit::iterator item=unit.begin(); item != unit.end(); item++) {
    if (0 < item->second) {
      if (item->second==1) {
        QString base = Fluc::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        nominator.push_back(new MathText(base));
      } else {
        QString base = Fluc::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        QString exponent = QString("%1").arg(item->second);
        nominator.push_back(new MathSup(new MathText(base),new MathText(exponent)));
      }
    } else {
      if (item->second==-1) {
        QString base = Fluc::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        denominator.push_back(new MathText(base));
      } else {
        QString base = Fluc::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        QString exponent = QString("%1").arg(std::abs(item->second));
        denominator.push_back(new MathSup(new MathText(base),new MathText(exponent)));
      }
    }
  }

  if ( (0 == nominator.size()) && (0 == denominator.size()) ) {
    _formula->appendItem(new MathText("a.u."));
    return;
  }

  if (0 == nominator.size()) { nominator.push_back(new MathText("1")); }

  if (0 == denominator.size()) {
    for (QList<MathFormulaItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
      _formula->appendItem(*item);
    }
    return;
  }

  MathFormula *nom = new MathFormula();
  for (QList<MathFormulaItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
    nom->appendItem(*item);
  }

  MathFormula *denom = new MathFormula();
  for (QList<MathFormulaItem *>::iterator item=denominator.begin(); item!=denominator.end(); item++) {
    denom->appendItem(*item);
  }

  _formula->appendItem(new MathFraction(nom, denom));
}


UnitRenderer::~UnitRenderer()
{
  delete _formula;
}


const QPixmap &
UnitRenderer::render()
{
  if (0 != _pixmap) { return *_pixmap; }

  // Render formula
  QGraphicsItem *rendered_formula = _formula->layout(MathContext()); rendered_formula->setPos(0,0);
  QGraphicsScene *scene = new QGraphicsScene();
  scene->addItem(rendered_formula);

  // Get size and draw formula on pixmap:
  QSize size = scene->sceneRect().size().toSize();
  _pixmap = new QPixmap(size.width(), size.height());
  QPainter painter(_pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  scene->render(&painter);
  delete scene;

  return *_pixmap;
}
