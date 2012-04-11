#include "linegraph.hh"
#include <iostream>
#include <limits>

using namespace Plot;



LineGraph::LineGraph(const GraphStyle &style)
  : Graph(style), path(), path_item(0), extent(0,1,0,1)
{
  // Construct path item for path:
  this->path_item = new QGraphicsPathItem();
  this->addToGroup(this->path_item);
}


Extent
LineGraph::getExtent()
{
  Extent extent = this->extent;

  if (0==this->extent.dX())
    extent = Extent(extent.minX()-1,extent.maxX()+1, extent.minY(), extent.maxY());

  if (0==this->extent.dY())
    extent = Extent(extent.minX(), extent.maxX(), extent.minY()-1, extent.maxY()+1);

  return extent;
}


void
LineGraph::addPoint(Eigen::VectorXd &point)
{
  this->addPoint(point(0), point(1));
}


void
LineGraph::addPoint(double x, double y)
{
  if (x != x || y != y)
  {
    std::cerr << __FILE__ << ": " << __LINE__
              << ": NaN occured during plot: Ignore." << std::endl;
    return;
  }

  if (0 == this->path.elementCount())
  {
    // Add first point to path:
    this->path.moveTo(x,y);
    this->extent = Extent(x,x, y,y);
  }
  else
  {
    // Extend path:
    this->path.lineTo(x,y);
    this->extent = this->extent.united(Extent(x,x,y,y));
  }
}


void
LineGraph::commit()
{
  emit this->graphModified();
}


void
LineGraph::updateGraph(Mapping &mapping)
{
  this->setPos(0,0);
  this->path_item->setPos(0,0);
  this->path_item->setPen(this->style.getPen());
  QPainterPath path = mapping(this->path);
  if (0 != path.length())
    this->path_item->setPath(path);
}

