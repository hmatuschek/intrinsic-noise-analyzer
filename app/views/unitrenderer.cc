#include "unitrenderer.hh"
#include <QPaintEngine>


UnitRenderer::UnitRenderer(const iNA::Ast::Unit &unit, QObject *parent)
  : QObject(parent), _formula(0), _pixmap(0)
{

    _formula = new MathFormula();

    // Catch time units first which refer explictly to minutes, hours or days
    if (unit.isVariantOf(iNA::Ast::ScaledBaseUnit::SECOND) && unit.size()==1)
    {
        double fac = unit.getMultiplier()*std::pow(10.,unit.getScale());

        if( fac==86400 )
        {
            _formula->appendItem(new MathText(QString("d")));
            return;
        }
        else if ( fac==3600 )
        {
            _formula->appendItem(new MathText(QString("h")));
            return;
        }
        else if ( fac==60 )
        {
            _formula->appendItem(new MathText(QString("min")));
            return;
        }
    }

  if (1 != unit.getMultiplier()) {
    _formula->appendItem(new MathText(QString("%2").arg(unit.getMultiplier())));
  }
  if (0 != unit.getScale()) {
    MathItem *item = new MathText(QString("%2").arg(unit.getScale()));
    _formula->appendItem(new MathSup(new MathText("10"), item));
  }

  QList<MathItem *> nominator;
  QList<MathItem *> denominator;
  for (iNA::Ast::Unit::iterator item=unit.begin(); item != unit.end(); item++) {
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
    _formula->appendItem(new MathText("a.u."));
    return;
  }

  if (0 == nominator.size()) { nominator.push_back(new MathText("1")); }

  if (0 == denominator.size()) {
    for (QList<MathItem *>::iterator item=nominator.begin(); item!=nominator.end(); item++) {
      _formula->appendItem(*item);
    }
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
}


UnitRenderer::~UnitRenderer()
{
  delete _formula;
}


const QPixmap &
UnitRenderer::toPixmap()
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


QPixmap
UnitRenderer::toPixmap(const iNA::Ast::Unit &unit)
{
  UnitRenderer renderer(unit); return renderer.toPixmap();
}

