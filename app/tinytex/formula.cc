#include "formula.hh"
#include <QFont>
#include <QFontInfo>
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


qreal MathContext::pixelSize() const { return QFontInfo(_font).pixelSize(); }
qreal MathContext::fontSize() const { return _font.pointSizeF(); }
void MathContext::setFontSize(qreal size) { _font.setPointSizeF(size); }
qreal MathContext::lineWidth() const { return QFontMetricsF(_font).lineWidth(); }


/* ******************************************************************************************** *
 * Implementation of MathMetrics
 * ******************************************************************************************** */
MathMetrics::MathMetrics()
  : _height(0), _width(0), _ascent(0), _center(0), _left_bearing(0), _right_bearing(0),
    _bbox(0,0,0,0)
{
  // pass...
}

MathMetrics::MathMetrics(const MathMetrics &other)
  : _height(other._height), _width(other._width), _ascent(other._ascent), _center(other._center),
    _left_bearing(other._left_bearing), _right_bearing(other._right_bearing),
    _bbox(other._bbox)
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of MathFormulaItem
 * ******************************************************************************************** */
MathFormulaItem::MathFormulaItem() : _metrics() {
  // pass..
}

MathFormulaItem::~MathFormulaItem() {
  // pass...
}

const MathMetrics &
MathFormulaItem::metrics() const {
  return _metrics;
}


/* ******************************************************************************************** *
 * Implementation of MathFormula
 * ******************************************************************************************** */
MathFormula::MathFormula() : MathFormulaItem() {
  // pass...
}

MathFormula::~MathFormula() {
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
  qreal max_center = 0;
  qreal max_tail = 0;
  qreal tot_width = 0;

  QGraphicsItemGroup *item_group = new QGraphicsItemGroup(parent); item_group->setPos(0,0);

  QList<QGraphicsItem *> item_list;
  // First traverse into all sub-elements with the same context and get max height of all elements:
  for (QList<MathFormulaItem *>::iterator item=_items.begin(); item!=_items.end(); item++) {
    // Layout formula item:
    QGraphicsItem *gitem = (*item)->layout(context, item_group);
    // add to list of graphics items:
    gitem->setPos(0,0); item_list.append(gitem);
    // Update measures depending on alignment of the item
    max_ascent = std::max(max_ascent, (*item)->metrics().ascent());
    max_center = std::max(max_center, (*item)->metrics().center());
    max_tail = std::max(max_tail, (*item)->metrics().center());
    tot_width += (*item)->metrics().width();
  }

  // Update width of forumla with bearings of the first and last element
  if (0 < _items.size()) {
    tot_width += -_items.first()->metrics().leftBearing();
    tot_width += -_items.back()->metrics().rightBearing();
    current_offset = -_items.first()->metrics().leftBearing();
  }


  // Now arrange item on base-line
  QRectF my_bb = _metrics.bb();
  QList<MathFormulaItem*>::iterator fitem = _items.begin();
  QList<QGraphicsItem *>::iterator gitem = item_list.begin();
  for (; fitem!=_items.end(); fitem++, gitem++) {
    qreal iw = (*fitem)->metrics().width();
    qreal ic = (*fitem)->metrics().center();
    QPointF item_pos;
    item_pos = QPointF(current_offset, max_center-ic);

    (*gitem)->setPos(item_pos);
    QRectF item_bb = (*fitem)->metrics().bb(); item_bb.translate(item_pos);
    my_bb = my_bb.unite(item_bb); current_offset += iw;
  }

  _metrics.setWidth(tot_width);
  _metrics.setHeight(max_center+max_tail);
  _metrics.setAscent(max_ascent);
  _metrics.setLeftBearing(0); _metrics.setRightBearing(0);
  _metrics.setBB(my_bb);
  _metrics.setCenter(max_center);

  // Done:
  return item_group;
}


/* ******************************************************************************************** *
 * Implementation of MathSpace
 * ******************************************************************************************** */
MathSpace::MathSpace(TeXSpace tex_space) : _factor(1) {
  switch (tex_space) {
  case THIN_SPACE:   _factor *= 3./18.; break;
  case MEDIUM_SPACE: _factor *= 4./18.; break;
  case THICK_SPACE:  _factor *= 5./18.; break;
  case QUAD_SPACE:   break;
  }
}

MathSpace::MathSpace(qreal factor) : _factor(factor) {
  // Pass...
}

MathSpace::~MathSpace() {
  // pass...
}

QGraphicsItem *
MathSpace::layout(const MathContext &context, QGraphicsItem *parent)
{
  _metrics.setHeight(1); _metrics.setAscent(1);
  _metrics.setWidth(context.pixelSize()*_factor);
  _metrics.setLeftBearing(0); _metrics.setRightBearing(0);
  _metrics.setBB(QRectF(0,0,_metrics.width(),_metrics.height()));
  return new QGraphicsTextItem(parent);
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

  MathContext sub_ctx(context); if (sub_ctx.fontSize() > 2) {
    sub_ctx.setFontSize(sub_ctx.fontSize()*0.75);
  }

  QGraphicsItem *nom_item = _nominator->layout(sub_ctx, item_group);
  QGraphicsItem *denom_item = _denominator->layout(sub_ctx, item_group);
  nom_item->setPos(0,0); denom_item->setPos(0,0);

  qreal tot_height = _nominator->metrics().bbHeight() + _denominator->metrics().bbHeight() + sub_ctx.lineWidth();
  qreal tot_width  = std::max(_nominator->metrics().bbWidth(), _denominator->metrics().bbWidth());

  QPointF nom_pos((tot_width - _nominator->metrics().bbWidth())/2, 0);

  QPointF denom_pos((tot_width-_denominator->metrics().bbWidth())/2,
                    _nominator->metrics().bbHeight());

  QRectF nom_bb = _nominator->metrics().bb(); nom_bb.translate(nom_pos);
  QRectF denom_bb = _nominator->metrics().bb(); denom_bb.translate(nom_pos);

  // Update own metrics:
  _metrics.setBB(nom_bb.unite(denom_bb));
  _metrics.setWidth(tot_width);
  _metrics.setHeight(tot_height);
  _metrics.setAscent(_nominator->metrics().bbHeight() + _denominator->metrics().bbAscent());
  _metrics.setCenter(_nominator->metrics().bbHeight() + sub_ctx.lineWidth()/2);
  _metrics.setLeftBearing(0); _metrics.setRightBearing(0);

  nom_item->setPos(nom_pos);
  denom_item->setPos(denom_pos);

  QGraphicsLineItem *line = new QGraphicsLineItem(0, 0, tot_width, 0, item_group);
  QPen pen = line->pen(); pen.setWidth(sub_ctx.lineWidth()); line->setPen(pen);
  line->setPos(0, _nominator->metrics().bbHeight());

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
  QGraphicsSimpleTextItem *item = new QGraphicsSimpleTextItem(parent); item->setPos(0,0);
  QFont font(item->font()); font.setPointSizeF(context.fontSize());

  // Update metrics:
  QFontMetricsF font_metrics(font);
  QRectF bb = font_metrics.boundingRect(_text);
  if ( 0 < _text.size() ) {
    _metrics.setLeftBearing(font_metrics.leftBearing(_text[0]));
    _metrics.setRightBearing(font_metrics.rightBearing(_text[_text.size()-1]));
  }
  _metrics.setWidth(bb.width()+_metrics.leftBearing()+_metrics.rightBearing());
  _metrics.setHeight(bb.height());
  _metrics.setAscent(-bb.top());
  _metrics.setBB(bb.translated(-bb.topLeft()));
  _metrics.setCenter(_metrics.ascent()/2);

  // Update item:
  item->setFont(font);
  item->setText(_text);

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

  QPointF base_pos(0, upper_height/2);
  QPointF upper_pos(base_width, 0);

  QRectF base_bb = _base->metrics().bb(); base_bb.translate(base_pos);
  QRectF upper_bb = _upper->metrics().bb(); upper_bb.translate(upper_pos);

  base_item->setPos(base_pos);
  upper_item->setPos(upper_pos);

  // Update metric:
  _metrics.setWidth(_base->metrics().width() + _upper->metrics().width());
  _metrics.setHeight(_base->metrics().height() + _upper->metrics().height()/2);
  _metrics.setAscent(_base->metrics().ascent() + _upper->metrics().height()/2);
  _metrics.setLeftBearing(_base->metrics().leftBearing());
  _metrics.setRightBearing(_upper->metrics().rightBearing());
  _metrics.setBB(base_bb.unite(upper_bb));
  _metrics.setCenter(_base->metrics().center() + _upper->metrics().bbHeight()/2);

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

  QPointF base_pos(0,0);
  QPointF lower_pos(base_width, _base->metrics().ascent()-lower_height/2);
  QRectF base_bb(_base->metrics().bb()); base_bb.translate(base_pos);
  QRectF lower_bb(_lower->metrics().bb()); lower_bb.translate(lower_pos);
  base_item->setPos(base_pos);
  lower_item->setPos(lower_pos);

  // Update metric:
  _metrics.setWidth(_base->metrics().width() + _lower->metrics().width());
  _metrics.setHeight(_base->metrics().ascent() +
                     std::max(_base->metrics().descent(), _lower->metrics().height()/2));
  _metrics.setAscent(_base->metrics().ascent());
  _metrics.setLeftBearing(_base->metrics().leftBearing());
  _metrics.setRightBearing(_lower->metrics().rightBearing());
  _metrics.setBB(base_bb.united(lower_bb));
  _metrics.setCenter(_base->metrics().center());

  return item_group;
}
