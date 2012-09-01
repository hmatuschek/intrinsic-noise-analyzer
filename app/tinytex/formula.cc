#include "formula.hh"
#include <QFont>
#include <QFontInfo>
#include <QFontMetricsF>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
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
MathItem::MathItem() : _metrics() {
  // pass..
}

MathItem::MathItem(const MathItem &other) : _metrics(other._metrics) {
  /* pass... */
}

MathItem::~MathItem() {
  // pass...
}

const MathMetrics &
MathItem::metrics() const {
  return _metrics;
}

QPixmap
MathItem::renderItem(const MathContext &ctx)
{
  QGraphicsScene *scene = new QGraphicsScene(); scene->addItem(layout(ctx));
  QSize size = scene->sceneRect().size().toSize();
  QPixmap pixmap(size.width(), size.height());
  QPainter painter(&pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  scene->render(&painter); delete scene;
  return pixmap;
}


/* ******************************************************************************************** *
 * Implementation of MathFormula
 * ******************************************************************************************** */
MathFormula::MathFormula() : MathItem() {
  // pass...
}

MathFormula::MathFormula(const MathFormula &other)
  : MathItem(other)
{
  for (QList<MathItem *>::const_iterator item=other._items.begin();
       item!=other._items.end(); item++)
  {
    _items.append((*item)->copy());
  }
}

MathFormula::~MathFormula() {
  for (QList<MathItem *>::iterator item=_items.begin(); item!=_items.end(); item++) {
    delete *item;
  }
}

size_t
MathFormula::size() const {
  return _items.size();
}

void
MathFormula::appendItem(MathItem *item) {
  _items.append(item);
}

void
MathFormula::prependItem(MathItem *item) {
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
  for (QList<MathItem *>::iterator item=_items.begin(); item!=_items.end(); item++) {
    // Layout formula item:
    QGraphicsItem *gitem = (*item)->layout(context, item_group);
    // add to list of graphics items:
    gitem->setPos(0,0); item_list.append(gitem);
    // Update measures depending on alignment of the item
    max_ascent = std::max(max_ascent, (*item)->metrics().ascent());
    max_center = std::max(max_center, (*item)->metrics().center());
    max_tail = std::max(max_tail, (*item)->metrics().tail());
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
  QList<MathItem*>::iterator fitem = _items.begin();
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


MathItem * MathFormula::copy() const { return new MathFormula(*this); }


/* ******************************************************************************************** *
 * Implementation of MathBlock
 * ******************************************************************************************** */
MathBlock::MathBlock(MathItem *center, MathItem *left, MathItem *right)
  : MathItem(), _center(center), _left(left), _right(right)
{
  // pass...
}

MathBlock::MathBlock(const MathBlock &other)
  : MathItem(other), _center(other._center->copy()), _left(0), _right(0)
{
  if (0 != other._left) { _left = other._left->copy(); }
  if (0 != other._right) { _right = other._right->copy(); }
}

MathBlock::~MathBlock()
{
  delete _center;
  if (0 != _left) { delete _left; }
  if (0 != _right) { delete _right; }
}

QGraphicsItem *
MathBlock::layout(const MathContext &context, QGraphicsItem *parent)
{
  QGraphicsItemGroup *grp = new QGraphicsItemGroup(parent);
  QGraphicsItem *center = _center->layout(context, grp);
  QGraphicsItem *left  = (0 != _left) ? _left->layout(context, grp): 0;
  QGraphicsItem *right = (0 != _right) ? _right->layout(context, grp): 0;

  qreal max_height = _center->metrics().bbHeight();
  qreal max_center = _center->metrics().center();
  qreal tot_width = _center->metrics().width();
  if (0 != _left) {
    max_height = std::max(max_height, _left->metrics().bbHeight());
    max_center = std::max(max_center, _left->metrics().center());
    tot_width  += _left->metrics().width();
  }
  if (0 != _right) {
    max_height = std::max(max_height, _right->metrics().bbHeight());
    max_center = std::max(max_center, _right->metrics().center());
    tot_width  += _right->metrics().width();
  }

  // Setup metrics:
  QRectF my_bb = metrics().bb();
  qreal shift = 0;
  if (0 != _left) {
    qreal iw = _left->metrics().width();
    qreal ic = _left->metrics().center();

    if (_left->metrics().bbHeight() < max_height) {
      qreal scale = max_height/_left->metrics().bbHeight()+0.1;
      ic = max_height/2;
      left->setTransform(QTransform().scale(1, scale));
    }

    QPointF item_pos;
    item_pos = QPointF(shift, max_center-ic);

    left->setPos(item_pos);
    QRectF item_bb = _left->metrics().bb(); item_bb.translate(item_pos);
    my_bb = my_bb.unite(item_bb); shift += iw;
  }

  {
    qreal iw = _center->metrics().width();
    qreal ic = _center->metrics().center();
    QPointF item_pos;
    item_pos = QPointF(shift, max_center-ic);

    center->setPos(item_pos);
    QRectF item_bb = _center->metrics().bb(); item_bb.translate(item_pos);
    my_bb = my_bb.unite(item_bb); shift += iw;
  }

  if (0 != _right) {
    qreal iw = _right->metrics().width();
    qreal ic = _right->metrics().center();

    if (_right->metrics().bbHeight() < max_height) {
      qreal scale = max_height/_right->metrics().bbHeight()+0.1;
      ic = max_height/2;
      right->setTransform(QTransform().scale(1, scale));
    }

    QPointF item_pos;
    item_pos = QPointF(shift, max_center-ic);

    right->setPos(item_pos);
    QRectF item_bb = _right->metrics().bb(); item_bb.translate(item_pos);
    my_bb = my_bb.unite(item_bb); shift += iw;
  }

  _metrics.setWidth(tot_width);
  _metrics.setHeight(max_height);
  _metrics.setAscent(max_center);
  _metrics.setLeftBearing(0); _metrics.setRightBearing(0);
  _metrics.setBB(my_bb);
  _metrics.setCenter(max_center);

  // Done:
  return grp;
}


MathItem *MathBlock::copy() const { return new MathBlock(*this); }


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

MathSpace::MathSpace(const MathSpace &other) :
  MathItem(other), _factor(other._factor)
{
  // pass...
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

MathItem *MathSpace::copy() const { return new MathSpace(*this); }


/* ******************************************************************************************** *
 * Implementation of MathFraction
 * ******************************************************************************************** */
MathFraction::MathFraction(MathItem *nom, MathItem *denom)
  : MathItem(), _nominator(nom), _denominator(denom)
{
}

MathFraction::MathFraction(const MathFraction &other)
  : MathItem(other), _nominator(other._nominator->copy()),
    _denominator(other._denominator->copy()) {
  // Pass...
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
                    _nominator->metrics().bbHeight()+sub_ctx.lineWidth());

  QRectF nom_bb = _nominator->metrics().bb(); nom_bb.translate(nom_pos);
  QRectF denom_bb = _denominator->metrics().bb(); denom_bb.translate(denom_pos);

  // Update own metrics:
  _metrics.setBB(nom_bb.unite(denom_bb));
  _metrics.setWidth(tot_width);
  _metrics.setHeight(tot_height);
  _metrics.setAscent(_nominator->metrics().bbHeight() + _denominator->metrics().bbHeight() + sub_ctx.lineWidth());
  _metrics.setCenter(_nominator->metrics().bbHeight() + sub_ctx.lineWidth()/2);
  _metrics.setLeftBearing(0); _metrics.setRightBearing(0);

  nom_item->setPos(nom_pos);
  denom_item->setPos(denom_pos);

  QGraphicsLineItem *line = new QGraphicsLineItem(0, 0, tot_width, 0, item_group);
  QPen pen = line->pen(); pen.setWidth(sub_ctx.lineWidth()); line->setPen(pen);
  line->setPos(0, _nominator->metrics().bbHeight());

  return item_group;
}


MathItem *
MathFraction::copy() const {
  return new MathFraction(*this);
}


/* ******************************************************************************************** *
 * Implementation of MathText
 * ******************************************************************************************** */
MathText::MathText(const QString &text)
  : MathItem(), _text(text)
{
  // Pass...
}

MathText::MathText(const MathText &other)
  : MathItem(other), _text(other._text)
{
  // Pass...
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

MathItem * MathText::copy() const { return new MathText(*this); }



/* ******************************************************************************************** *
 * Implementation of MathText
 * ******************************************************************************************** */
MathSymbol::MathSymbol(QChar symbol)
  : MathItem(), _symbol(symbol) {
  // pass...
}

MathSymbol::MathSymbol(const MathSymbol &other)
  : MathItem(other), _symbol(other._symbol) {
  // pass...
}

MathSymbol::~MathSymbol() {
  // Pass...
}

QGraphicsItem *
MathSymbol::layout(const MathContext &context, QGraphicsItem *parent)
{
  // Create item and get font:
  QGraphicsSimpleTextItem *item = new QGraphicsSimpleTextItem(parent); item->setPos(0,0);
  QFont font(item->font()); font.setPointSizeF(context.fontSize());

  // Update metrics:
  QFontMetricsF font_metrics(font);
  QRectF char_bb = font_metrics.boundingRect(_symbol);
  QRectF str_bb = font_metrics.boundingRect(QString(_symbol));
  qreal voffset = -char_bb.top()-char_bb.height();

  qDebug() << "MathSymbol " << _symbol << " : " << char_bb;
  qDebug() << "  as string: " << str_bb;
  qDebug() << "  voffset: " << voffset;
  qDebug() << "  string ascent: " << -(str_bb.top()+voffset);
  qDebug() << "  string center: " << -(str_bb.top()+voffset+char_bb.height()/2);
  _metrics.setLeftBearing(font_metrics.leftBearing(_symbol));
  _metrics.setRightBearing(font_metrics.rightBearing(_symbol));
  _metrics.setWidth(str_bb.width()+_metrics.leftBearing()+_metrics.rightBearing());
  _metrics.setHeight(str_bb.height());
  _metrics.setAscent(-str_bb.top());
  _metrics.setBB(char_bb.translated(-char_bb.topLeft()));
  _metrics.setCenter(_metrics.ascent()-voffset-char_bb.height()/2);
  qDebug() << " final bb: " << _metrics.bb();
  // Update item:
  item->setFont(font);
  item->setText(_symbol);

  return item;
}

MathItem * MathSymbol::copy() const { return new MathSymbol(*this); }



/* ******************************************************************************************** *
 * Implementation of MathSup
 * ******************************************************************************************** */
MathSup::MathSup(MathItem *base, MathItem *upper)
  : MathItem(), _base(base), _upper(upper)
{
  // pass...
}

MathSup::MathSup(const MathSup &other)
  : MathItem(other), _base(other._base->copy()), _upper(other._upper->copy())
{
  // Pass...
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


MathItem *MathSup::copy() const { return new MathSup(*this); }



/* ******************************************************************************************** *
 * Implementation of MathSub: X_Y
 * ******************************************************************************************** */
MathSub::MathSub(MathItem *base, MathItem *lower)
  : MathItem(), _base(base), _lower(lower)
{
  // pass...
}

MathSub::MathSub(const MathSub &other)
  : MathItem(other), _base(other._base->copy()), _lower(other._lower->copy())
{
  // Pass...
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


MathItem * MathSub::copy() const { return new MathSub(*this); }
