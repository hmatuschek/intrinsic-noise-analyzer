#include "legend.hh"
#include "plot.hh"
#include "configuration.hh"

using namespace Plot;



/* ********************************************************************************************* *
 * Implementation of LegendItem:
 * ********************************************************************************************* */
LegendItem::LegendItem(const QString &label, Graph *graph, QObject *parent)
  : QObject(parent), QGraphicsItemGroup(), graph(graph)
{
  this->setPos(0,0);

  this->space = 5;
  this->sample_length = 20;

  this->line = new QGraphicsLineItem(0,0, this->sample_length, 0);
  this->line->setPen(this->graph->getStyle().getPen());
  this->addToGroup(this->line);
  this->line->setPos(0,0);

  this->label = new QGraphicsTextItem(label);
  this->label->setFont(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).legentFont());
  this->addToGroup(this->label);
  this->label->setPos(0,0);

  this->updateLayout();
}


void
LegendItem::setScheme(Configuration::Scheme scheme)
{
  // First, update label-font:
  this->label->setFont(
        Configuration::getConfig()->getScheme(scheme).legentFont());
  // Update line-style:
  this->line->setPen(this->graph->getStyle().getPen());

  // Update layout:
  this->updateLayout();
}


QRectF
LegendItem::boundingRect() const
{
  return this->bb;
}


void
LegendItem::updateLayout()
{
  // Reset...
  this->line->setPos(0,0);
  this->label->setPos(0,0);

  // Update positions:
  this->line->setPos(0, this->label->boundingRect().height()/2 - this->line->pen().width()/2);
  this->label->setPos(this->sample_length+this->space, 0);

  // recalc BB:
  this->bb = this->line->boundingRect();
  this->bb.setX(this->bb.x() + this->line->pos().x());
  this->bb.setY(this->bb.y() + this->line->pos().y());
  this->bb = this->bb.united(
        QRectF(this->label->boundingRect().x()+this->label->pos().x(),
               this->label->boundingRect().y()+this->label->pos().y(),
               this->label->boundingRect().width(),
               this->label->boundingRect().height()));
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
