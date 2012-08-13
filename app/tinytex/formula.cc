#include "formula.hh"
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QDebug>

#include <iostream>


/* ******************************************************************************************** *
 * Implementation of MathContext
 * ******************************************************************************************** */
MathContext::MathContext()
  : _font()
{
  // pass...
}

MathContext::MathContext(const MathContext &other)
  : _font(other._font)
{
  // Pass...
}


qreal MathContext::pixelSize() const { return _font.pixelSize(); }
qreal MathContext::fontSize() const { return _font.pointSizeF(); }
void MathContext::setFontSize(qreal size) { _font.setPointSizeF(size); }
qreal MathContext::lineWidth() const { return QFontMetricsF(_font).lineWidth(); }


/* ******************************************************************************************** *
 * Implementation of MathMetrics
 * ******************************************************************************************** */
MathMetrics::MathMetrics()
  : _ascent(0), _descent(0), _width(0), _left_bearing(0), _right_bearing(0), _bb_size(0,0)
{
  // pass...
}

MathMetrics::MathMetrics(const MathMetrics &other)
  : _ascent(other._ascent), _descent(other._descent), _width(other._width),
    _left_bearing(other._left_bearing), _right_bearing(other._right_bearing),
    _bb_size(other._bb_size)
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of MathFormulaItem
 * ******************************************************************************************** */
MathFormulaItem::MathFormulaItem()
  : _metrics()
{
  // pass..
}

MathFormulaItem::~MathFormulaItem()
{
  // pass...
}

const MathMetrics &
MathFormulaItem::metrics() const
{
  return _metrics;
}


/* ******************************************************************************************** *
 * Implementation of MathFormula
 * ******************************************************************************************** */
MathFormula::MathFormula()
  : MathFormulaItem()
{
  // pass...
}

MathFormula::~MathFormula()
{
  for (QList<MathFormulaItem *>::iterator item=_items.begin(); item!=_items.end(); item++) {
    delete *item;
  }
}

size_t
MathFormula::size() const {
  return _items.size();
}

void
MathFormula::appendItem(MathFormulaItem *item) {
  _items.append(item);
}

void
MathFormula::prependItem(MathFormulaItem *item) {
  _items.prepend(item);
}


QGraphicsItem*
MathFormula::layout(const MathContext &context, QGraphicsItem *parent)
{
  qreal current_offset = 0;
  qreal max_ascent = 0;
  qreal max_descent = 0;
  qreal tot_width = 0;

  QGraphicsItemGroup *item_group = new QGraphicsItemGroup(parent); item_group->setPos(0,0);

  QList<QGraphicsItem *> item_list;
  // First traverse into all sub-elements with the same context and get max height of all elements:
  for (QList<MathFormulaItem *>::iterator item=_items.begin(); item!=_items.end(); item++) {
    QGraphicsItem *gitem = (*item)->layout(context, item_group);
    gitem->setPos(0,0); item_list.append(gitem);
    max_ascent = std::max(max_ascent, (*item)->metrics().ascent());
    max_descent = std::max(max_descent, (*item)->metrics().descent());
    tot_width += (*item)->metrics().width();
  }

  // Now arrange item on center-line
  QList<MathFormulaItem*>::iterator fitem = _items.begin();
  QList<QGraphicsItem *>::iterator gitem = item_list.begin();
  for (; fitem!=_items.end(); fitem++, gitem++) {
    qreal ia = (*fitem)->metrics().ascent();
    qreal iw = (*fitem)->metrics().width();
    (*gitem)->setPos(current_offset, (max_ascent - ia));
    current_offset += iw;
  }

  _metrics.setWidth(tot_width); _metrics.setAscent(max_ascent); _metrics.setDescent(max_descent);
  _metrics.setBBSize(_metrics.size());

  qDebug() << "Layout formular " << _metrics.bbWidth() << "x" << _metrics.bbHeight();
  // Done:
  return item_group;
}



/* ******************************************************************************************** *
 * Implementation of MathFraction
 * ******************************************************************************************** */
MathFraction::MathFraction(MathFormulaItem *nom, MathFormulaItem *denom)
  : MathFormulaItem(), _nominator(nom), _denominator(denom)
{
}

MathFraction::~MathFraction()
{
  delete _nominator;
  delete _denominator;
}


QGraphicsItem*
MathFraction::layout(const MathContext &context, QGraphicsItem *parent)
{
  QGraphicsItemGroup *item_group = new QGraphicsItemGroup(parent); item_group->setPos(0,0);
  //item_group->addToGroup(new QGraphicsEllipseItem(0,0,2,2));

  MathContext sub_ctx(context); if (sub_ctx.fontSize() > 2) {
    sub_ctx.setFontSize(sub_ctx.fontSize()*0.75);
  }

  QGraphicsItem *nom_item = _nominator->layout(sub_ctx, item_group);
  QGraphicsItem *denom_item = _denominator->layout(sub_ctx, item_group);
  QGraphicsLineItem *line = new QGraphicsLineItem(item_group);
  QPen pen = line->pen(); pen.setWidth(sub_ctx.lineWidth()); line->setPen(pen);
  nom_item->setPos(0,0); denom_item->setPos(0,0); line->setPos(0,0);

  qreal space=5;
  qreal tot_height = _nominator->metrics().bbHeight() + space + _denominator->metrics().bbHeight();
  qreal tot_width  = std::max(_nominator->metrics().bbWidth(), _denominator->metrics().bbWidth());

  // Update own metrics:
  _metrics.setWidth(tot_width);
  _metrics.setAscent(_nominator->metrics().bbHeight()+space+_denominator->metrics().ascent());
  _metrics.setBBSize(_metrics.size());

  nom_item->setPos((tot_width - _nominator->metrics().width())/2, 0);
  denom_item->setPos((tot_width-_denominator->metrics().width())/2,
                     space+_nominator->metrics().bbHeight());
  line->setLine(0, tot_height/2, tot_width, tot_height/2);

  std::cerr << "Layout frac: " << _metrics.bbWidth() << "x" << _metrics.bbHeight() << std::endl;

  return item_group;
}



/* ******************************************************************************************** *
 * Implementation of MathText
 * ******************************************************************************************** */
MathText::MathText(const QString &text)
  : MathFormulaItem(), _text(text)
{
}

MathText::~MathText()
{
  // pass...
}

QGraphicsItem *
MathText::layout(const MathContext &context, QGraphicsItem *parent)
{
  // Create item and get font:
  QGraphicsTextItem *item = new QGraphicsTextItem(parent); item->setPos(0,0);
  QFont font(item->font()); font.setPointSizeF(context.fontSize());

  // Update metrics:
  QFontMetricsF font_metrics(font);
  _metrics.setWidth(font_metrics.width(_text));
  _metrics.setAscent(font_metrics.ascent());
  _metrics.setDescent(font_metrics.descent());
  _metrics.setBBSize(font_metrics.boundingRect(_text).size());

  // Update item:
  item->setFont(font);
  item->setPlainText(_text);

  qDebug() << QString("Layout text %1: %2x%3").arg(_text).arg(_metrics.bbWidth()).arg(_metrics.bbHeight());
  return item;
}



/* ******************************************************************************************** *
 * Implementation of MathSup
 * ******************************************************************************************** */
MathSup::MathSup(MathFormulaItem *base, MathFormulaItem *upper)
  : MathFormulaItem(), _base(base), _upper(upper)
{
  // pass...
}

MathSup::~MathSup() {
  delete _base;
  delete _upper;
}

QGraphicsItem*
MathSup::layout(const MathContext &context, QGraphicsItem *parent)
{
  QGraphicsItemGroup *item_group = new QGraphicsItemGroup(parent);
  MathContext sup_ctx(context);
  if (sup_ctx.fontSize() > 2) {
    sup_ctx.setFontSize(sup_ctx.fontSize() * 0.667);
  }

  // Layout sub-elements:
  QGraphicsItem *base_item =_base->layout(context, item_group);
  QGraphicsItem *upper_item =_upper->layout(sup_ctx, item_group);

  base_item->setPos(0,0); item_group->addToGroup(base_item);
  upper_item->setPos(0,0); item_group->addToGroup(upper_item);

  qreal base_width = _base->metrics().width();
  qreal upper_height = _upper->metrics().bbHeight();

  base_item->setPos(0, upper_height/2);
  upper_item->setPos(base_width, 0);

  // Update metric:
  _metrics.setWidth(_base->metrics().width() + _upper->metrics().width());
  _metrics.setAscent(_base->metrics().ascent() + _upper->metrics().height()/2);
  _metrics.setDescent(_base->metrics().descent());

  _metrics.setBBSize(
        QSizeF(_base->metrics().width() + _upper->metrics().width(),
               _base->metrics().bbHeight() + _upper->metrics().bbHeight()/2));

  return item_group;
}



/* ******************************************************************************************** *
 * Implementation of MathSub: X_Y
 * ******************************************************************************************** */
MathSub::MathSub(MathFormulaItem *base, MathFormulaItem *lower)
  : MathFormulaItem(), _base(base), _lower(lower)
{
  // pass...
}

MathSub::~MathSub() {
  delete _base;
  delete _lower;
}

QGraphicsItem*
MathSub::layout(const MathContext &context, QGraphicsItem *parent)
{
  QGraphicsItemGroup *item_group = new QGraphicsItemGroup(parent);
  MathContext sub_ctx(context);
  if (sub_ctx.fontSize() > 2) {
    sub_ctx.setFontSize(sub_ctx.fontSize()*0.667);
  }

  // Layout sub-elements:
  QGraphicsItem *base_item = _base->layout(context, item_group);
  QGraphicsItem *lower_item = _lower->layout(sub_ctx, item_group);

  base_item->setPos(0,0); item_group->addToGroup(base_item);
  lower_item->setPos(0,0); item_group->addToGroup(lower_item);

  qreal base_width = _base->metrics().width();
  qreal lower_height = _lower->metrics().bbHeight();

  base_item->setPos(0, 0);
  lower_item->setPos(base_width, _base->metrics().bbHeight()-lower_height/2);

  // Update metric:
  _metrics.setWidth(_base->metrics().width() + _lower->metrics().width());
  _metrics.setAscent(_base->metrics().ascent());
  _metrics.setDescent(_base->metrics().descent()+_lower->metrics().height()/2);

  _metrics.setBBSize(
        QSizeF(_base->metrics().width() + _lower->metrics().width(),
               _base->metrics().bbHeight() + _lower->metrics().bbHeight()/2));

  return item_group;
}
