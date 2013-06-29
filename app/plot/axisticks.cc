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
  this->label->setDefaultTextColor(QColor(Qt::black));
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
  this->label->setDefaultTextColor(QColor(Qt::black));
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
  : QGraphicsItemGroup(), _orientation(orientation), _ticks(), _mapping(mapping), _labeltext(label),
    _labelitem(0)
{
  // Render label:
  _mathcontext = MathContext(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).axisLabelFont());

  // Draw axis line:
  _axis_line = new QGraphicsLineItem(0, 0, 0, 0);
  _axis_line->setPen(
        Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).defaultPen());
  addToGroup(_axis_line);

  // Layout all
  updateRange();
}


void
AxisTicks::addTick(AxisTick *tick) {
  _ticks.append(tick);
  addToGroup(_ticks.back());
}

qreal
AxisTicks::lineWidth() const {
  return _axis_line->pen().width();
}

void
AxisTicks::updateRange() {
  updateLables();
  updatePositions();
}


void
AxisTicks::updateSize() {
  updatePositions();
}


void
AxisTicks::updateLables()
{
  // remove old label:
  if (0 != _labelitem) {
    removeFromGroup(_labelitem); delete _labelitem;
  }

  // Render label:
  MathItem *label_item = TinyTex::parseInlineQuoted(_labeltext.toStdString());
  _labelitem = label_item->layout(_mathcontext); _labelsize = label_item->metrics().size();
  _labelitem->setPos(0,0); delete label_item;
  addToGroup(_labelitem);

  // Rotate label by orientation:
  switch (this->_orientation)
  {
  case AxisTick::LEFT:
  case AxisTick::RIGHT:
    _labelitem->setRotation(-90.0);
    break;

  case AxisTick::TOP:
  case AxisTick::BOTTOM:
    _labelitem->setRotation(0.0);
    break;
  }
}


void
AxisTicks::updatePositions()
{
  double max_width = 0.0;
  double max_height = 0.0;
  for (int i=0; i<this->_ticks.size(); i++)
  {
    max_width  = std::max(max_width, this->_ticks[i]->boundingRect().width());
    max_height = std::max(max_height, this->_ticks[i]->boundingRect().height());
  }

  // Update label position of left or above ticks:
  double margin = 0;
  switch (this->_orientation)
  {
  case AxisTick::LEFT:
    this->_labelitem->setPos(0, _mapping->size().height()/2 + _labelsize.width()/2);
    margin += _labelsize.height();
    break;

  case AxisTick::TOP:
    this->_labelitem->setPos(this->_mapping->size().width()/2 - this->_labelsize.width()/2, 0);
    margin += _labelsize.height();
    break;

  default:
    break;
  }

  // Update position of ticks:
  for (int i=0; i<this->_ticks.size(); i++)
  {
    double width  = _ticks[i]->boundingRect().width();
    double height = _ticks[i]->boundingRect().height();

    double x = 0; double y = 0;
    switch (_orientation)
    {
    case AxisTick::LEFT:
      x = max_width - width + margin;
      y = (*(_mapping))(QPointF(0, _ticks[i]->getValue())).y() - height/2;
      break;

    case AxisTick::RIGHT:
      x = 0;
      y = (*(_mapping))(QPointF(0, _ticks[i]->getValue())).y() - height/2;
      break;

    case AxisTick::TOP:
      x = (*(_mapping))(QPointF(_ticks[i]->getValue(), 0)).x() - width/2;
      y = max_height-height + margin;
      break;

    case AxisTick::BOTTOM:
      x = (*(_mapping))(QPointF(_ticks[i]->getValue(), 0)).x() - width/2;
      y = 0;
      break;
    }

    _ticks[i]->setPos(x,y);
  }

  // Update label position if right or below ticks:
  switch (_orientation)
  {
  case AxisTick::RIGHT:
    _labelitem->setPos(max_width, _mapping->size().height()/2+_labelsize.width()/2);
    break;

  case AxisTick::BOTTOM:
    _labelitem->setPos(_mapping->size().width()/2 - _labelsize.width()/2, max_height);
    break;

  default:
    break;
  }

  // Update axis-line:
  switch (this->_orientation)
  {
  case AxisTick::LEFT:
    _axis_line->setLine(max_width+margin, 0, max_width+margin, _mapping->size().height());
    break;

  case AxisTick::RIGHT:
    _axis_line->setLine(0, 0, 0, _mapping->size().height());
    break;

  case AxisTick::TOP:
    _axis_line->setLine(0, max_height+margin, _mapping->size().width(), max_height+margin);
    break;

  case AxisTick::BOTTOM:
    _axis_line->setLine(0,0, _mapping->size().width(),0);
    break;
  }
}


void
AxisTicks::setScheme(Configuration::Scheme scheme)
{
  // Update math context & label
  _mathcontext = MathContext(Configuration::getConfig()->getScheme(scheme).axisLabelFont());

  // Update axis-line pen:
  _axis_line->setPen(Configuration::getConfig()->getScheme(scheme).defaultPen());

  // Update Ticks:
  for (int i=0; i<_ticks.size(); i++) {
    _ticks[i]->setScheme(scheme);
  }

  updateLables();
  updatePositions();
}


void
AxisTicks::setLabel(const QString &label)
{
  // Set label
  _labeltext = label;
  updateLables();
  updatePositions();
}


QRectF
AxisTicks::boundingRect() const
{
  QRectF box(0,0,0,0);

  // Update bounding box by orientation:
  switch (_orientation)
  {
  case AxisTick::LEFT:
  case AxisTick::RIGHT:
    box = box.united(
          QRectF(_labelitem->pos().x(), _labelitem->pos().y(),
                 _labelsize.height(), _labelsize.width()));
    break;

  case AxisTick::TOP:
  case AxisTick::BOTTOM:
    box = box.united(
          QRectF(_labelitem->pos().x(), _labelitem->pos().y(),
                 _labelsize.width(), _labelsize.height()));
  }

  // unify box with all bounding boxes of all ticks:
  for (int i=0; i<_ticks.size(); i++)
  {
    box = box.united(
          QRectF(_ticks[i]->boundingRect().x() + _ticks[i]->pos().x(),
                 _ticks[i]->boundingRect().y() + _ticks[i]->pos().y(),
                 _ticks[i]->boundingRect().width(),
                 _ticks[i]->boundingRect().height()));
  }

  // Finally unify with axis line:
  return box.united(
        QRectF(_axis_line->boundingRect().x() + _axis_line->pos().x(),
               _axis_line->boundingRect().y() + _axis_line->pos().y(),
               _axis_line->boundingRect().width(),
               _axis_line->boundingRect().height()));
}




/* ********************************************************************************************* *
 * Auto Axis ticks
 * ********************************************************************************************* */
AutoAxisTicks::AutoAxisTicks(Mapping *mapping, size_t num_ticks,
                             AxisTick::Orientation orientation, const QString &label)
  : AxisTicks(mapping, orientation, label)
{
  Eigen::VectorXd values(num_ticks);
  switch (this->_orientation)
  {
    case AxisTick::LEFT:
    case AxisTick::RIGHT:
      this->_mapping->sampleY(values);
      break;

    case AxisTick::BOTTOM:
    case AxisTick::TOP:
      this->_mapping->sampleX(values);
      break;
  }

  // Allocate space for ticks:
  this->_ticks.resize(num_ticks);

  for (size_t i=0; i<num_ticks; i++)
  {
    this->_ticks[i] = new AxisTick(values(i), this->_orientation);
    this->addToGroup(this->_ticks[i]);
  }

  // Update
  this->updateRange();
}


void
AutoAxisTicks::updateLables()
{
  // Call default handler...
  AxisTicks::updateLables();

  // Update labels of ticks:
  Eigen::VectorXd values(_ticks.size());
  switch (_orientation)
  {
  case AxisTick::LEFT:
  case AxisTick::RIGHT:
    _mapping->sampleY(values);
    break;

  case AxisTick::TOP:
  case AxisTick::BOTTOM:
    _mapping->sampleX(values);
    break;
  }

  for (int i=0; i<this->_ticks.size(); i++)
  {
    // Update value and label of all ticks:
    _ticks[i]->setValue(values(i), true);
  }
}


