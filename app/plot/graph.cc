#include "graph.hh"
#include "axis.hh"
#include <iostream>

using namespace Plot;



/* ******************************************************************************************** *
 * Implementation of GraphStyle
 * ******************************************************************************************** */
GraphStyle::GraphStyle()
  : pen(), fillColor(pen.color())
{
}


GraphStyle::GraphStyle(Qt::PenStyle style, const QColor &color, double width)
  : pen(), fillColor(color)
{
  this->pen.setStyle(style);
  this->pen.setBrush(color);
  this->pen.setWidth(width);
  this->pen.setCosmetic(true);
  this->pen.setJoinStyle(Qt::MiterJoin);
  this->pen.setCapStyle(Qt::SquareCap);
}


GraphStyle::GraphStyle(const GraphStyle &other)
  : pen(other.pen), fillColor(other.fillColor)
{
  // Pass...
}


const QPen &
GraphStyle::getPen() const
{
  return this->pen;
}


void
GraphStyle::setPenStyle(Qt::PenStyle style)
{
  this->pen.setStyle(style);
}


void
GraphStyle::setLineWidth(double w)
{
  this->pen.setWidthF(w);
}


const QColor &
GraphStyle::getFillColor() const
{
  return this->fillColor;
}

QColor
GraphStyle::getLineColor() const
{
  return this->pen.color();
}


void
GraphStyle::setFillColor(const QColor &color)
{
  this->fillColor = color;
}

void
GraphStyle::setLineColor(const QColor &color)
{
  this->pen.setBrush(color);
}



/* ******************************************************************************************** *
 * Implementation of Graph
 * ******************************************************************************************** */
Graph::Graph(const GraphStyle &style, QObject *parent)
  : QObject(parent), style(style)
{
}


const GraphStyle &
Graph::getStyle() const
{
  return this->style;
}


void
Graph::setScheme(Configuration::Scheme scheme)
{
  // Update Pen-width:
  this->style.setLineWidth(
        Configuration::getConfig()->getScheme(scheme).defaultPen().widthF());
}

