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
  : width(0), height(0)
{
  // pass...
}

MathMetrics::MathMetrics(const MathMetrics &other)
  : width(other.width), height(other.height)
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
MathFormula::addItem(MathFormulaItem *item) {
  _items.append(item);
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
    max_height = std::max(max_height, (*item)->metrics().height);
    tot_width += (*item)->metrics().width+space;
  }

  _metrics.width = tot_width;
  _metrics.height = max_height;

  // Now arrange item on center-line
  QList<MathFormulaItem*>::iterator fitem = _items.begin();
  QList<QGraphicsItem *>::iterator gitem = item_list.begin();
  for (; fitem!=_items.end(); fitem++, gitem++) {
    qreal ih = (*fitem)->metrics().height;
    qreal iw = (*fitem)->metrics().width;
    (*gitem)->setPos(current_offset, (max_height - ih)/2);
    current_offset += iw + space;
  }

  std::cerr << "Rendered formula with " << _metrics.width << "x" << _metrics.height << std::endl;

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
  qreal tot_height = _nominator->metrics().height + space + _denominator->metrics().height;
  qreal tot_width  = std::max(_nominator->metrics().width, _denominator->metrics().width);

  // Update own metrics:
  _metrics.height = tot_height;
  _metrics.width  = tot_width;

  nom_item->setPos((tot_width - _nominator->metrics().width)/2, 0);
  denom_item->setPos((tot_width-_denominator->metrics().width)/2,
                     space+_nominator->metrics().height);
  line->setLine(3, tot_height-space/2, tot_width+3, tot_height-space/2);

  std::cerr << "Rendered frac with " << _metrics.width << "x" << _metrics.height << std::endl;

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
  _metrics.height = font_metrics.tightBoundingRect(_text).height();
  _metrics.width = font_metrics.tightBoundingRect(_text).width();

  // Update item:
  item->setFont(font);
  item->setPlainText(_text);

  std::cerr << "Rendered text with " << _metrics.width << "x" << _metrics.height << std::endl;

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

  qreal base_width = _base->metrics().width;
  qreal upper_height = _upper->metrics().height;

  base_item->setPos(0, upper_height/2);
  upper_item->setPos(base_width, 0);

  // Update metric:
  _metrics.width = _base->metrics().width + _upper->metrics().width;
  _metrics.height = _base->metrics().height + _upper->metrics().height/2;

  std::cerr << "Rendered sup with " << _metrics.width << "x" << _metrics.height << std::endl;

  return item_group;
}
