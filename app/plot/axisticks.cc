#include "axisticks.hh"
#include <QTextStream>
#include "plot.hh"
#include "configuration.hh"

#include <iostream>


using namespace Plot;


/* ********************************************************************************************* *
 * Simple Axis Tick
 * ********************************************************************************************* */
AxisTick::AxisTick(double value, Orientation orientation)
  : QGraphicsItemGroup(), value(value), orientation(orientation)
{
  // Construct label:
  this->label = new QGraphicsTextItem(QString("%1").arg(value));
  this->label->setFont(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).ticksFont());
  this->label->setTextInteractionFlags(Qt::NoTextInteraction);
  this->addToGroup(this->label);

  this->tick = new QGraphicsLineItem(0,0,0,0);
  this->tick->setPen(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).defaultPen());

  this->addToGroup(this->tick);

  // move tick:
  double h = this->label->boundingRect().height();
  double w = this->label->boundingRect().width();

  switch (this->orientation)
  {
  case LEFT:
    this->label->setPos(0,0);
    this->tick->setLine(w, h/2, w+5, h/2);
    break;

  case RIGHT:
    this->label->setPos(5, 0);
    this->tick->setLine(0, h/2, 5, h/2);
    break;

  case TOP:
    this->label->setPos(0,0);
    this->tick->setLine(w/2, h, w/2, h+5);
    break;

  case BOTTOM:
    this->label->setPos(0, 5);
    this->tick->setLine(w/2, 0, w/2, 5);
    break;
  }
}


AxisTick::AxisTick(double value, const QString &label, Orientation orientation)
  : QGraphicsItemGroup(), value(value), orientation(orientation)
{
  // Construct label:
  this->label = new QGraphicsTextItem(label);
  this->label->setFont(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).ticksFont());
  this->label->setTextInteractionFlags(Qt::NoTextInteraction);
  this->addToGroup(this->label);

  this->tick = new QGraphicsLineItem(0,0,0,0);
  this->tick->setPen(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).defaultPen());
  this->addToGroup(this->tick);

  // move tick:
  double h = this->label->boundingRect().height();
  double w = this->label->boundingRect().width();

  switch (this->orientation)
  {
  case LEFT:
    this->label->setPos(0,0);
    this->tick->setLine(w, h/2, w+5, h/2);
    break;

  case RIGHT:
    this->label->setPos(5, 0);
    this->tick->setLine(0, h/2, 5, h/2);
    break;

  case TOP:
    this->label->setPos(0,0);
    this->tick->setLine(w/2, h, w/2, h+5);
    break;

  case BOTTOM:
    this->label->setPos(0, 5);
    this->tick->setLine(w/2, 0, w/2, 5);
    break;
  }
}


void
AxisTick::setLabel(const QString &label)
{
  // Update label text:
  this->label->setPlainText(label);
  /// \todo Update label font!

  // move tick:
  double h = this->label->boundingRect().height();
  double w = this->label->boundingRect().width();
  /// \todo Update tick-pen.

  switch (this->orientation)
  {
  case LEFT:
    this->label->setPos(0,0);
    this->tick->setLine(w, h/2, w+5, h/2);
    break;

  case RIGHT:
    this->label->setPos(5, 0);
    this->tick->setLine(0, h/2, 5, h/2);
    break;

  case TOP:
    this->label->setPos(0,0);
    this->tick->setLine(w/2, h, w/2, h+5);
    break;

  case BOTTOM:
    this->label->setPos(0, 5);
    this->tick->setLine(w/2, 0, w/2, 5);
    break;
  }
}


QString
AxisTick::getLabel()
{
  return this->label->toPlainText();
}


QRectF
AxisTick::boundingRect() const
{
  QRectF box(0,0,0,0);

  box = box.united(
        QRectF(this->label->boundingRect().x() + this->label->pos().x(),
               this->label->boundingRect().y() + this->label->pos().y(),
               this->label->boundingRect().width(),
               this->label->boundingRect().height()));

  box = box.united(
        QRectF(this->tick->boundingRect().x() + this->tick->pos().x(),
               this->tick->boundingRect().y() + this->tick->pos().y(),
               this->tick->boundingRect().width(),
               this->tick->boundingRect().height()));

  return box;
}


double
AxisTick::getValue() const
{
  return this->value;
}


void
AxisTick::setValue(double value, bool update_label)
{
  this->value = value;

  if (update_label)
  {
    this->setLabel(QString("%1").arg(value));
  }
}


void
AxisTick::setScheme(Configuration::Scheme scheme)
{
  // Update label font:
  this->label->setFont(Configuration::getConfig()->getScheme(scheme).ticksFont());
  // Update tick-pen:
  this->tick->setPen(Configuration::getConfig()->getScheme(scheme).defaultPen());
}



/* ********************************************************************************************* *
 * Axis ticks
 * ********************************************************************************************* */
AxisTicks::AxisTicks(Mapping *mapping, AxisTick::Orientation orientation, const QString &label)
  : QGraphicsItemGroup(), orientation(orientation), ticks(), mapping(mapping)
{
  this->label_item = new QGraphicsTextItem(label);
  this->label_item->setFont(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).axisLabelFont());
  this->label_item->setPos(0,0);
  this->addToGroup(this->label_item);

  // Rotate label by orientation:
  switch (this->orientation)
  {
  case AxisTick::LEFT:
  case AxisTick::RIGHT:
    this->label_item->setRotation(-90.0);
    break;

  case AxisTick::TOP:
  case AxisTick::BOTTOM:
    this->label_item->setRotation(0.0);
    break;
  }

  // Draw axis line:
  this->axis_line = new QGraphicsLineItem(0, 0, 0, 0);
  this->axis_line->setPen(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).defaultPen());
  this->addToGroup(this->axis_line);

  this->updateRange();
}


void
AxisTicks::addTick(AxisTick *tick)
{
  this->ticks.append(tick);
  this->addToGroup(this->ticks.back());
}


void
AxisTicks::updateRange()
{
  this->updateLables();
  this->updatePositions();
}


void
AxisTicks::updateSize()
{
  this->updatePositions();
}


void
AxisTicks::updateLables()
{
  // Pass...
}


void
AxisTicks::updatePositions()
{
  double max_width = 0.0;
  double max_height = 0.0;
  for (int i=0; i<this->ticks.size(); i++)
  {
    max_width  = std::max(max_width, this->ticks[i]->boundingRect().width());
    max_height = std::max(max_height, this->ticks[i]->boundingRect().height());
  }

  // Update label position of left or above ticks:
  double margin = 0;
  switch (this->orientation)
  {
  case AxisTick::LEFT:
    this->label_item->setPos(0, this->mapping->size().height()/2 + this->label_item->boundingRect().width()/2);
    margin += this->label_item->boundingRect().height();
    break;

  case AxisTick::TOP:
    this->label_item->setPos(this->mapping->size().width()/2 - this->label_item->boundingRect().width()/2, 0);
    margin += this->label_item->boundingRect().height();
    break;

  default:
    break;
  }

  // Update position of ticks:
  for (int i=0; i<this->ticks.size(); i++)
  {
    double width  = this->ticks[i]->boundingRect().width();
    double height = this->ticks[i]->boundingRect().height();

    double x = 0; double y = 0;
    switch (this->orientation)
    {
    case AxisTick::LEFT:
      x = max_width - width + margin;
      y = (*(this->mapping))(QPointF(0, this->ticks[i]->getValue())).y() - height/2;
      break;

    case AxisTick::RIGHT:
      x = 0;
      y = (*(this->mapping))(QPointF(0, this->ticks[i]->getValue())).y() - height/2;
      break;

    case AxisTick::TOP:
      x = (*(this->mapping))(QPointF(this->ticks[i]->getValue(), 0)).x() - width/2;
      y = max_height-height + margin;
      break;

    case AxisTick::BOTTOM:
      x = (*(this->mapping))(QPointF(this->ticks[i]->getValue(), 0)).x() - width/2;
      y = 0;
      break;
    }

    this->ticks[i]->setPos(x,y);
  }

  // Update label position if right or below ticks:
  switch (this->orientation)
  {
  case AxisTick::RIGHT:
    this->label_item->setPos(max_width, this->mapping->size().height()/2+this->label_item->boundingRect().width()/2);
    break;

  case AxisTick::BOTTOM:
    this->label_item->setPos(this->mapping->size().width()/2 - this->label_item->boundingRect().width()/2, max_height);
    break;

  default:
    break;
  }

  // Update axis-line:
  switch (this->orientation)
  {
  case AxisTick::LEFT:
    this->axis_line->setLine(max_width+margin, 0, max_width+margin, this->mapping->size().height());
    break;

  case AxisTick::RIGHT:
    this->axis_line->setLine(0, 0, 0, this->mapping->size().height());
    break;

  case AxisTick::TOP:
    this->axis_line->setLine(0, max_height+margin, this->mapping->size().width(), max_height+margin);
    break;

  case AxisTick::BOTTOM:
    this->axis_line->setLine(0,0,this->mapping->size().width(),0);
    break;
  }
}


void
AxisTicks::setScheme(Configuration::Scheme scheme)
{
  // Update axis-label scheme:
  this->label_item->setFont(Configuration::getConfig()->getScheme(scheme).axisLabelFont());
  // Update axis-line pen:
  this->axis_line->setPen(Configuration::getConfig()->getScheme(scheme).defaultPen());

  // Update Ticks:
  for (int i=0; i<this->ticks.size(); i++) {
    this->ticks[i]->setScheme(scheme);
  }
}


void
AxisTicks::setLabel(const QString &label)
{
  this->label_item->setHtml(label);
  /// Update label-font.
}


QRectF
AxisTicks::boundingRect() const
{
  QRectF box(0,0,0,0);

  // Update bounding box by orientation:
  switch (this->orientation)
  {
  case AxisTick::LEFT:
  case AxisTick::RIGHT:
    box = box.united(
          QRectF(this->label_item->boundingRect().x() + this->label_item->pos().x(),
                 this->label_item->boundingRect().y() + this->label_item->pos().y(),
                 this->label_item->boundingRect().height(),
                 this->label_item->boundingRect().width()));
    break;

  case AxisTick::TOP:
  case AxisTick::BOTTOM:
    box = box.united(
          QRectF(this->label_item->boundingRect().x() + this->label_item->pos().x(),
                 this->label_item->boundingRect().y() + this->label_item->pos().y(),
                 this->label_item->boundingRect().width(),
                 this->label_item->boundingRect().height()));
  }

  // unify box with all bounding boxes of all ticks:
  for (int i=0; i<this->ticks.size(); i++)
  {
    box = box.united(
          QRectF(this->ticks[i]->boundingRect().x() + this->ticks[i]->pos().x(),
                 this->ticks[i]->boundingRect().y() + this->ticks[i]->pos().y(),
                 this->ticks[i]->boundingRect().width(),
                 this->ticks[i]->boundingRect().height()));
  }

  // Finally unify with axis line:
  return box.united(
        QRectF(this->axis_line->boundingRect().x() + this->axis_line->pos().x(),
               this->axis_line->boundingRect().y() + this->axis_line->pos().y(),
               this->axis_line->boundingRect().width(),
               this->axis_line->boundingRect().height()));
}




/* ********************************************************************************************* *
 * Auto Axis ticks
 * ********************************************************************************************* */
AutoAxisTicks::AutoAxisTicks(Mapping *mapping, size_t num_ticks,
                             AxisTick::Orientation orientation, const QString &label)
  : AxisTicks(mapping, orientation, label)
{
  Eigen::VectorXd values(num_ticks);
  switch (this->orientation)
  {
    case AxisTick::LEFT:
    case AxisTick::RIGHT:
      this->mapping->sampleY(values);
      break;

    case AxisTick::BOTTOM:
    case AxisTick::TOP:
      this->mapping->sampleX(values);
      break;
  }

  // Allocate space for ticks:
  this->ticks.resize(num_ticks);

  for (size_t i=0; i<num_ticks; i++)
  {
    this->ticks[i] = new AxisTick(values(i), this->orientation);
    this->addToGroup(this->ticks[i]);
  }

  // Update
  this->updateRange();
}


void
AutoAxisTicks::updateLables()
{
  // Update labels of ticks:
  Eigen::VectorXd values(this->ticks.size());
  switch (this->orientation)
  {
  case AxisTick::LEFT:
  case AxisTick::RIGHT:
    this->mapping->sampleY(values);
    break;

  case AxisTick::TOP:
  case AxisTick::BOTTOM:
    this->mapping->sampleX(values);
    break;
  }

  for (int i=0; i<this->ticks.size(); i++)
  {
    // Update value and label of all ticks:
    this->ticks[i]->setValue(values(i), true);
  }
}


