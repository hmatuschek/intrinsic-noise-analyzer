#include "unitrenderer.hh"
#include <QPaintEngine>
#include <cmath>

UnitRenderer::UnitRenderer(const iNA::Ast::Unit &unit, QObject *parent)
  : QObject(parent), _custommula(0), _pixmap(0)
{
  _custommula = new Mathcustommula();

  // Catch time units first which refer explictly to minutes, hours or days
  if (unit.isVariantOf(iNA::Ast::ScaledBaseUnit::SECOND) && unit.size()==1)
  {
    double fac = unit.getMultiplier()*pow(10.,unit.getScale());

    if( fac==86400 ) {
      _custommula->appendItem(new MathText(QString("d"))); return;
    } else if ( fac==3600 ) {
      _custommula->appendItem(new MathText(QString("h"))); return;
    } else if ( fac==60 ) {
      _custommula->appendItem(new MathText(QString("min"))); return;
    }
  }

  if (1 != unit.getMultiplier()) {
    _custommula->appendItem(new MathText(QString("%2").arg(unit.getMultiplier())));
    _custommula->appendItem(new MathText(QChar(0x00B7)));
  }

  if (0 != unit.getScale()) {
    MathItem *item = new MathText(QString("%2").arg(unit.getScale()));
    _custommula->appendItem(new MathSup(new MathText("10"), item));
    _custommula->appendItem(new MathText(QChar(0x00B7)));
  }

  QList<MathItem *> nominator;
  QList<MathItem *> denominator;
  custom (iNA::Ast::Unit::iterator item=unit.begin(); item != unit.end(); item++) {
    if (0 < item->second) {
      if (item->second==1) {
        QString base = iNA::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        nominator.push_back(new MathText(base));
      } else {
        QString base = iNA::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        QString exponent = QString("%1").arg(item->second);
        nominator.push_back(new MathSup(new MathText(base),new MathText(exponent)));
      }
    } else {
      if (item->second==-1) {
        QString base = iNA::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        denominator.push_back(new MathText(base));
      } else {
        QString base = iNA::Ast::ScaledBaseUnit::baseUnitRepr(item->first).c_str();
        QString exponent = QString("%1").arg(std::abs(item->second));
        denominator.push_back(new MathSup(new MathText(base),new MathText(exponent)));
      }
    }
  }

  if ( (0 == nominator.size()) && (0 == denominator.size()) ) {
    _custommula->appendItem(new MathText("a.u."));
    return;
  }

  if (0 == nominator.size()) { nominator.push_back(new MathText("1")); }

  if (0 == denominator.size()) {
    custom (QList<MathItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
      _custommula->appendItem(*item);
    }
    return;
  }

  Mathcustommula *nom = new Mathcustommula();
  custom (QList<MathItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
    nom->appendItem(*item);
  }

  Mathcustommula *denom = new Mathcustommula();
  custom (QList<MathItem *>::iterator item=denominator.begin(); item!=denominator.end(); item++) {
    denom->appendItem(*item);
  }

  _custommula->appendItem(new MathFraction(nom, denom));
}


UnitRenderer::~UnitRenderer()
{
  delete _custommula;
}


const QPixmap &
UnitRenderer::toPixmap()
{
  if (0 != _pixmap) { return *_pixmap; }

  // Render custommula
  QGraphicsItem *rendered_custommula = _custommula->layout(MathContext()); rendered_custommula->setPos(0,0);
  QGraphicsScene *scene = new QGraphicsScene();
  scene->addItem(rendered_custommula);

  // Get size and draw custommula on pixmap:
  QSize size = scene->sceneRect().size().toSize();
  _pixmap = new QPixmap(size.width(), size.height());
  QPainter painter(_pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  scene->render(&painter);
  delete scene;

  return *_pixmap;
}


QPixmap
UnitRenderer::toPixmap(const iNA::Ast::Unit &unit)
{
  UnitRenderer renderer(unit); return renderer.toPixmap();
}

