#include "variancelinegraph.hh"
#include <iostream>
using namespace Plot;


VarianceLineGraph::VarianceLineGraph(const GraphStyle &style)
  : Graph(style), path(), upper_var_path(), lower_var_path(), path_item(0), var_path_item(0),
    extent(0,1,0,1)
{
  QColor fcolor = this->style.getFillColor(); fcolor.setAlpha(32); this->style.setFillColor(fcolor);

  // Construct path item for path:
  this->path_item = new QGraphicsPathItem();
  this->var_path_item = new QGraphicsPathItem();

  this->addToGroup(this->var_path_item);
  this->addToGroup(this->path_item);
}


Extent
VarianceLineGraph::getExtent()
{
  Extent extent = this->extent;

  if (0==this->extent.dX())
    extent = Extent(extent.minX()-1,extent.maxX()+1, extent.minY(), extent.maxY());

  if (0==this->extent.dY())
    extent = Extent(extent.minX(), extent.maxX(), extent.minY()-1, extent.maxY()+1);

  return extent;
}


void
VarianceLineGraph::addPoint(Eigen::VectorXd &point)
{
  this->addPoint(point(0), point(1), point(2));
}


void
VarianceLineGraph::addPoint(double x, double y, double s)
{
  // Check for NaNs:
  if (x != x || y != y || s != s) {
    std::cerr << __FILE__ << ":" << __LINE__
              << ": NaN occured in plot." << std::endl;
    return;
  }

  if (0 == this->path.elementCount())
  {
    // Add first point to path:
    this->path.moveTo(x,y);

    this->upper_var_path.moveTo(x,y+s);
    this->lower_var_path.moveTo(x,y-s);

    this->extent = Extent(x,x, y-s,y+s);
  }
  else
  {
    // Extend path:
    this->path.lineTo(x,y);

    this->upper_var_path.lineTo(x,y+s);
    this->lower_var_path.lineTo(x,y-s);

    this->extent = this->extent.united(Extent(x,x,y-s,y+s));
  }
}


void
VarianceLineGraph::commit()
{
  emit this->graphModified();
}


void
VarianceLineGraph::updateGraph(Mapping &mapping)
{
  // Set position of graph:
  this->setPos(0,0);

  // Handle variance area as a closed path of the upper and lower variance
  // paths:
  this->var_path_item->setPos(0,0);
  this->var_path_item->setPen(QPen(Qt::transparent));
  this->var_path_item->setBrush(this->getStyle().getFillColor());
  // Map paths into parent (axis) coordinates:
  QPainterPath tmp = mapping(this->upper_var_path);
  tmp.connectPath(mapping(this->lower_var_path.toReversed()));
  // If there is a path to plot:
  if (0 != tmp.length())
    this->var_path_item->setPath(tmp);

  // Handle "mean" path:
  this->path_item->setPos(0,0);
  this->path_item->setPen(this->style.getPen());
  // Map mean into parent coordinates:
  QPainterPath path = mapping(this->path);
  // if there is a path to plot:
  if (0 != path.length())
    this->path_item->setPath(path);
}
