#include "unitrenderer.hh"
#include <QPaintEngine>
#include <cmath>

UnitRenderer::UnitRenderer(const iNA::Ast::Unit &unit, QObject *parent)
  : QObject(parent), _formula(0), _scene(0), _pixmap(0)
{
  setUnit(unit);
}


UnitRenderer::~UnitRenderer()
{
  delete _formula;
  delete _scene;
}


void
UnitRenderer::setUnit(const iNA::Ast::Unit &unit)
{
  // Clear pre-rendered formulas:
  if (0 != _formula) { delete _formula; }
  if (0 != _scene) { delete _scene; }

  // Render Unit:
  _formula = new MathFormula();

  // Catch time units first which refer explictly to minutes, hours or days
  if (unit.isVariantOf(iNA::Ast::Unit::SECOND) && unit.size()==1)
  {
    double fac = unit.getMultiplier()*pow(10.,unit.getScale());

    if( fac==86400 ) {
      _formula->appendItem(new MathText(QString("d"))); _renderFormula(); return;
    } else if ( fac==3600 ) {
      _formula->appendItem(new MathText(QString("h"))); _renderFormula(); return;
    } else if ( fac==60 ) {
      _formula->appendItem(new MathText(QString("min"))); _renderFormula(); return;
    }
  }

  if (1 != unit.getMultiplier()) {
    _formula->appendItem(new MathText(QString("%2").arg(unit.getMultiplier())));
    _formula->appendItem(new MathText(QChar(0x00B7)));
  }

  if (0 != unit.getScale()) {
    MathItem *item = new MathText(QString("%2").arg(unit.getScale()));
    _formula->appendItem(new MathSup(new MathText("10"), item));
    _formula->appendItem(new MathText(QChar(0x00B7)));
  }

  QList<MathItem *> nominator;
  QList<MathItem *> denominator;
  for (iNA::Ast::Unit::iterator item=unit.begin(); item != unit.end(); item++) {
    if (0 < item->second) {
      if (item->second==1) {
        QString base = iNA::Ast::Unit::baseUnitRepr(item->first).c_str();
        nominator.push_back(new MathText(base));
      } else {
        QString base = iNA::Ast::Unit::baseUnitRepr(item->first).c_str();
        QString exponent = QString("%1").arg(item->second);
        nominator.push_back(new MathSup(new MathText(base),new MathText(exponent)));
      }
    } else {
      if (item->second==-1) {
        QString base = iNA::Ast::Unit::baseUnitRepr(item->first).c_str();
        denominator.push_back(new MathText(base));
      } else {
        QString base = iNA::Ast::Unit::baseUnitRepr(item->first).c_str();
        QString exponent = QString("%1").arg(std::abs(item->second));
        denominator.push_back(new MathSup(new MathText(base),new MathText(exponent)));
      }
    }
  }

  if ( (0 == nominator.size()) && (0 == denominator.size()) ) {
    _formula->appendItem(new MathText("a.u."));
    _renderFormula();
    return;
  }

  if (0 == nominator.size()) { nominator.push_back(new MathText("1")); }

  if (0 == denominator.size()) {
    for (QList<MathItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
      _formula->appendItem(*item);
    }
    _renderFormula();
    return;
  }

  MathFormula *nom = new MathFormula();
  for (QList<MathItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
    nom->appendItem(*item);
  }

  MathFormula *denom = new MathFormula();
  for (QList<MathItem *>::iterator item=denominator.begin(); item!=denominator.end(); item++) {
    denom->appendItem(*item);
  }

  _formula->appendItem(new MathFraction(nom, denom));
  _renderFormula();
}


void
UnitRenderer::_renderFormula() {
  QGraphicsItem *rendered_formula = _formula->layout(MathContext());
  rendered_formula->setPos(0,0);
  _scene = new QGraphicsScene();
  _scene->addItem(rendered_formula);
}


QSize
UnitRenderer::size() const {
  return _scene->sceneRect().size().toSize();
}

const QPixmap &
UnitRenderer::toPixmap()
{
  if (0 != _pixmap) { return *_pixmap; }

  // Get size and draw formula on pixmap:
  QSize size = _scene->sceneRect().size().toSize();
  _pixmap = new QPixmap(size.width(), size.height());
  QPainter painter(_pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  _scene->render(&painter);

  return *_pixmap;
}


void
UnitRenderer::renderOn(QPainter *painter, const QRect &target, const QRect &source)
{
  // Get size and draw formula on pixmap:
  _scene->render(painter, target, source);
}

QPixmap
UnitRenderer::toPixmap(const iNA::Ast::Unit &unit)
{
  UnitRenderer renderer(unit); return renderer.toPixmap();
}

void
UnitRenderer::renderOn(const iNA::Ast::Unit &unit, QPainter *painter) {
  UnitRenderer renderer(unit); renderer.renderOn(painter);
}
