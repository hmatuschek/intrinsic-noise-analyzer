#include "legend.hh"
#include "plot.hh"
#include "configuration.hh"
#include "../tinytex/tinytex.hh"
#include <QDebug>

using namespace Plot;



/* ********************************************************************************************* *
 * Implementation of LegendItem:
 * ********************************************************************************************* */
LegendItem::LegendItem(const QString &label, Graph *graph, QObject *parent)
  : QObject(parent), QGraphicsItemGroup(), _graph(graph), _label_text(label),
    _space(5), _sample_length(20)
{
  _line = new QGraphicsLineItem(0,0, _sample_length, 0);
  _line->setPen(_graph->getStyle().getPen());
  _line->setPos(0,0); addToGroup(_line);

  MathItem *label_item = TinyTex::parseInlineQuoted(_label_text.toStdString());
  MathContext context(Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).legendFont());
  _label = label_item->layout(context);
  _label_size = label_item->metrics().size(); delete label_item;
  _label->setPos(0,0); addToGroup(_label);

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
  _label->setPos(0,0); addToGroup(_label);
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
  // Reset positions
  prepareGeometryChange();
  _line->setPos(0,0); _label->setPos(0,0);

  // Update positions:
  _line->setPos(0, _label_size.height()/2);
  _label->setPos(_sample_length+_space, 0);

  // update BB:
  _bb = _line->boundingRect();
  _bb.setX(0); _bb.setY(0);
  _bb = _bb.united(QRectF(_sample_length+_space, 0, _label_size.width(), _label_size.height()));

  qDebug() << "Legend Item @ " << _bb;
}



/* ********************************************************************************************* *
 * Implementation of Legend:
 * ********************************************************************************************* */
Legend::Legend(QObject *parent)
  : QObject(parent), QGraphicsItemGroup()
{
  this->_margin_left   = 10;
  this->_margin_bottom = 5;
  this->_margin_right  = 10;
  this->_margin_top    = 5;
  this->_line_spacing  = 3;

  this->_background = new QGraphicsPathItem();
  this->_background->setPos(0,0);
  this->_background->setPen(QColor(Qt::transparent));
  this->_background->setBrush(QColor(255,255,255, 200));
  this->addToGroup(this->_background);
}



void
Legend::addGraph(const QString &label, Graph *graph)
{
  LegendItem *item = new LegendItem(label, graph);

  double x = this->_margin_left;
  double y = this->_margin_top;

  if (0 < this->_items.size())
  {
    y = this->_items.last()->boundingRect().y() +
        this->_items.last()->boundingRect().height() +
        this->_items.last()->pos().y() +
        this->_line_spacing;
  }

  this->_items.append(item);
  this->addToGroup(item);
  item->setPos(x,y);
  //qDebug() << "Created legend item @ " << x << ", " << y;
  item->updateLayout();
  this->updateBB();
}


QRectF
Legend::boundingRect() const {
  return this->_bb;
}


void
Legend::setScheme(Configuration::Scheme scheme) {
  for(QList<LegendItem *>::iterator item = this->_items.begin(); item != this->_items.end(); item++) {
    (*item)->setScheme(scheme);
  }
  updateLayout();
}


void
Legend::updateLayout() {
  double x = this->_margin_left, y = this->_margin_top;
  for (QList<LegendItem *>::Iterator item=_items.begin(); item!=_items.end(); item++) {
    //qDebug() << "Legend: move item to " << x << ", " << y;
    (*item)->setPos(x,y); (*item)->updateLayout();
    y = (*item)->pos().y() + (*item)->boundingRect().y() + (*item)->boundingRect().height() + _line_spacing;
  }
  updateBB();
}


void
Legend::updateBB()
{
  QRectF box(0,0,0,0);

  for (QList<LegendItem *>::iterator item = this->_items.begin();
       item != this->_items.end(); item++)
  {
    box = box.united(
          QRectF((*item)->boundingRect().x() + (*item)->pos().x(),
                 (*item)->boundingRect().y() + (*item)->pos().y(),
                 (*item)->boundingRect().width() + this->_margin_right,
                 (*item)->boundingRect().height() + this->_margin_bottom));
  }

  this->_bb = box;

  // Update background
  QPainterPath path; path.addRect(0,0, box.width(), box.height());
  this->_background->setPath(path);
}
