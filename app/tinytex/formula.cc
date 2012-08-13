#include "formula.hh"
#include <QFont>
#include <QFontMetrics>
#include <iostream>


/* ******************************************************************************************** *
 * Implementation of MathContext
 * ******************************************************************************************** */
MathContext::MathContext()
  : fontSize(12)
{
  // pass...
}

MathContext::MathContext(const MathContext &other)
  : fontSize(other.fontSize)
{
  // Pass...
}


/* ******************************************************************************************** *
 * Implementation of MathMetrics
 * ******************************************************************************************** */
MathMetrics::MathMetrics()
  : _size(0,0), _bb_size(0,0)
{
  // pass...
}

MathMetrics::MathMetrics(const MathMetrics &other)
  : _size(other._size), _bb_size(other._bb_size)
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
  qreal current_offset = 3, space=3;
  qreal max_height = 0; qreal tot_width = 3;

  QGraphicsItemGroup *item_group = new QGraphicsItemGroup(parent); item_group->setPos(0,0);

  QList<QGraphicsItem *> item_list;
  // First traverse into all sub-elements with the same context and get max height of all elements:
  for (QList<MathFormulaItem *>::iterator item=_items.begin(); item!=_items.end(); item++) {
    QGraphicsItem *gitem = (*item)->layout(context, item_group);
    gitem->setPos(0,0); item_list.append(gitem);
    max_height = std::max(max_height, (*item)->metrics().height());
    tot_width += (*item)->metrics().width()+space;
  }

  _metrics.setSize(QSizeF(tot_width, max_height));
  _metrics.setBBSize(_metrics.size());

  // Now arrange item on center-line
  QList<MathFormulaItem*>::iterator fitem = _items.begin();
  QList<QGraphicsItem *>::iterator gitem = item_list.begin();
  for (; fitem!=_items.end(); fitem++, gitem++) {
    qreal ih = (*fitem)->metrics().height();
    qreal iw = (*fitem)->metrics().width();
    (*gitem)->setPos(current_offset, (max_height - ih)/2);
    current_offset += iw + space;
  }

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

  MathContext sub_ctx(context); if (sub_ctx.fontSize > 2) { sub_ctx.fontSize -= sub_ctx.fontSize/4; }

  QGraphicsItem *nom_item = _nominator->layout(sub_ctx, item_group);
  QGraphicsItem *denom_item = _denominator->layout(sub_ctx, item_group);
  QGraphicsLineItem *line = new QGraphicsLineItem(item_group);

  nom_item->setPos(0,0); denom_item->setPos(0,0); line->setPos(0,0);

  qreal space=5;
  qreal tot_height = _nominator->metrics().height() + space + _denominator->metrics().height();
  qreal tot_width  = std::max(_nominator->metrics().width(), _denominator->metrics().width());

  // Update own metrics:
  _metrics.setSize(QSizeF(tot_height, tot_width));
  _metrics.setBBSize(_metrics.size());

  nom_item->setPos((tot_width - _nominator->metrics().width())/2, 0);
  denom_item->setPos((tot_width-_denominator->metrics().width())/2,
                     space+_nominator->metrics().height());
  line->setLine(3, tot_height-space/2, tot_width+3, tot_height-space/2);

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
  QFont font(item->font()); font.setPointSize(context.fontSize);

  // Update metrics:
  QFontMetrics font_metrics(font);
  _metrics.setSize(font_metrics.tightBoundingRect(_text).size());
  _metrics.setBBSize(_metrics.size());

  // Update item:
  item->setFont(font);
  item->setPlainText(_text);

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
  MathContext sup_ctx(context); if (sup_ctx.fontSize > 2) { sup_ctx.fontSize -= sup_ctx.fontSize/3; }

  // Layout sub-elements:
  QGraphicsItem *base_item =_base->layout(context, item_group);
  QGraphicsItem *upper_item =_upper->layout(sup_ctx, item_group);

  base_item->setPos(0,0); item_group->addToGroup(base_item);
  upper_item->setPos(0,0); item_group->addToGroup(upper_item);

  qreal base_width = _base->metrics().width();
  qreal upper_height = _upper->metrics().height();

  base_item->setPos(0, upper_height/2);
  upper_item->setPos(base_width, 0);

  // Update metric:
  _metrics.setSize(
        QSizeF(_base->metrics().width() + _upper->metrics().width(),
               _base->metrics().height() + _upper->metrics().height()/2));
  _metrics.setBBSize(_metrics.size());

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
  MathContext sub_ctx(context); if (sub_ctx.fontSize > 2) { sub_ctx.fontSize -= sub_ctx.fontSize/3; }

  // Layout sub-elements:
  QGraphicsItem *base_item = _base->layout(context, item_group);
  QGraphicsItem *lower_item = _lower->layout(sub_ctx, item_group);

  base_item->setPos(0,0); item_group->addToGroup(base_item);
  lower_item->setPos(0,0); item_group->addToGroup(lower_item);

  qreal base_width = _base->metrics().width();
  qreal lower_height = _lower->metrics().height();

  base_item->setPos(0, 0);
  lower_item->setPos(base_width, _base->metrics().height()-lower_height/2);

  // Update metric:
  _metrics.setSize(
        QSizeF(_base->metrics().width() + _lower->metrics().width(),
               _base->metrics().height() + _lower->metrics().height()/2));
  _metrics.setBBSize(_metrics.size());

  return item_group;
}
