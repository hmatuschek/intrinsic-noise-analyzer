#include "legend.hh"
#include "plot.hh"
#include "configuration.hh"
#include "../tinytex/tinytex.hh"

using namespace Plot;



/* ********************************************************************************************* *
 * Implementation of LegendItem:
 * ********************************************************************************************* */
LegendItem::LegendItem(const QString &label, Graph *graph, QObject *parent)
  : QObject(parent), QGraphicsItemGroup(), _graph(graph), _label_text(label),
    _space(5), _sample_length(20)
{
  this->setPos(0,0);

  _line = new QGraphicsLineItem(0,0, _sample_length, 0);
  _line->setPen(_graph->getStyle().getPen());
  addToGroup(_line);
  _line->setPos(0,0);

  MathItem *label_item = TinyTex::parseInlineQuoted(_label_text.toStdString());
  MathContext context(Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).legendFont());
  _label = label_item->layout(context);
  _label_size = label_item->metrics().size(); delete label_item;
  addToGroup(_label);
  _label->setPos(0,0);

  updateLayout();
}


void
LegendItem::setScheme(Configuration::Scheme scheme)
{
  prepareGeometryChange();
  // First remove "old" lable
  removeFromGroup(_label); delete _label;
  // Create new label with scheme
  MathItem *label_item = TinyTex::parseInlineQuoted(_label_text.toStdString());
  MathContext context(Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).legendFont());
  _label = label_item->layout(context);
  _label_size = label_item->metrics().size(); delete label_item;
  addToGroup(_label);
  // Update line-style:
  _line->setPen(_graph->getStyle().getPen());
  // Update layout:
  updateLayout();
}


QRectF
LegendItem::boundingRect() const {
  return this->_bb;
}


void
LegendItem::updateLayout()
{
  // Reset...
  this->_line->setPos(0,0);
  this->_label->setPos(0,0);

  // Update positions:
  this->_line->setPos(0, _label_size.height()/2 - _line->pen().width()/2);
  this->_label->setPos(_sample_length+_space, 0);

  // recalc BB:
  this->_bb = this->_line->boundingRect();
  this->_bb.setX(this->_bb.x() + this->_line->pos().x());
  this->_bb.setY(this->_bb.y() + this->_line->pos().y());
  this->_bb = this->_bb.united(
        QRectF(_label->pos().x(), _label->pos().y(),
               _label_size.width(), _label_size.height()));
}



/* ********************************************************************************************* *
 * Implementation of Legend:
 * ********************************************************************************************* */
Legend::Legend(QObject *parent)
  : QObject(parent), QGraphicsItemGroup()
{
  this->margin_left   = 10;
  this->margin_bottom = 5;
  this->margin_right  = 10;
  this->margin_top    = 5;
  this->line_spacing  = 3;

  this->setPos(0,0);
  this->background = new QGraphicsPathItem();
  this->background->setPos(0,0);
  this->background->setPen(QColor(Qt::transparent));
  this->background->setBrush(QColor(255,255,255, 200));

  this->addToGroup(this->background);
}



void
Legend::addGraph(const QString &label, Graph *graph)
{
  LegendItem *item = new LegendItem(label, graph);
  item->setPos(0,0);

  double x = this->margin_left;
  double y = this->margin_top;

  if (0 < this->items.size())
  {
    y = this->items.last()->boundingRect().y() +
        this->items.last()->boundingRect().height() +
        this->items.last()->pos().y() +
        this->line_spacing;
  }

  this->items.append(item);
  this->addToGroup(item);
  item->setPos(x,y);
  // Update bounding box:
  this->updateBB();
}


QRectF
Legend::boundingRect() const
{
  return this->bb;
}


void
Legend::setScheme(Configuration::Scheme scheme)
{
  for(QList<LegendItem *>::iterator item = this->items.begin();
      item != this->items.end(); item++)
  {
    (*item)->setScheme(scheme);
  }
}


void
Legend::updateBB()
{
  QRectF box(0,0,0,0);

  for (QList<LegendItem *>::iterator item = this->items.begin();
       item != this->items.end(); item++)
  {
    (*item)->updateLayout();
    box = box.united(
          QRectF((*item)->boundingRect().x() + (*item)->pos().x(),
                 (*item)->boundingRect().y() + (*item)->pos().y(),
                 (*item)->boundingRect().width() + this->margin_right,
                 (*item)->boundingRect().height() + this->margin_bottom));
  }

  this->bb = box;

  // Update path
  QPainterPath path; path.addRect(0,0, box.width(), box.height());
  this->background->setPath(path);
}
