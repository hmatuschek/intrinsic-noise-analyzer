#include "axis.hh"
#include "graph.hh"


using namespace Plot;


/* ********************************************************************************************* *
 * Implementation of MeasureItem
 * ********************************************************************************************* */
MeasureItem::MeasureItem(Axis *parent)
  : QObject(parent), QGraphicsItemGroup(), _values(0,0)
{
  setPos(0,0);

  _label = new QGraphicsTextItem(this);
  _label->setPos(3,-3);

  new QGraphicsLineItem(-5, 0, 5, 0, this);
  new QGraphicsLineItem(0, -5, 0, 5, this);

  addToGroup(_label);
  setVisible(false);
}

void
MeasureItem::setValues(const QPointF &values)
{
  _values = values;
  _label->setPlainText(QString("%1, %2").arg(_values.x()).arg(_values.y()));
}

const QPointF & MeasureItem::getValues() const { return _values; }



/* ********************************************************************************************* *
 * Implementation of GraphGroup
 * ********************************************************************************************* */
GraphGroup::GraphGroup(QObject *parent)
  : QObject(parent), QGraphicsItemGroup()
{
  // Clip all drawing (graphs) to the axis:
  this->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
}


void
GraphGroup::setClipping(const QRectF &rect)
{
  this->_clipping = rect;
}


QRectF
GraphGroup::boundingRect() const
{
  return this->_clipping;
}



/* ********************************************************************************************* *
 * Implementation of Axis class
 * ********************************************************************************************* */
Axis::Axis(QObject *parent)
  : QObject(parent), QGraphicsItemGroup(), mapping(0), axis_size(),
    xticks(0), yticks(0), graph_group(0)
{
  this->mapping = new Mapping(Extent(0,0,1,1), QSizeF(1,1), this);
  this->xticks = new AutoAxisTicks(this->mapping, 5, AxisTick::BOTTOM);
  this->xticks->setPos(0,0);
  this->addToGroup(this->xticks);

  this->yticks = new AutoAxisTicks(this->mapping, 5, AxisTick::LEFT);
  this->yticks->setPos(0,0);
  this->addToGroup(this->yticks);

  QPen zero_pen(Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).defaultPen());
  zero_pen.setColor(QColor(128, 128, 128, 64));

  this->x_zero = new QGraphicsLineItem(0,0,0,0);
  this->x_zero->setPen(zero_pen);
  this->x_zero->setVisible(false);

  this->y_zero = new QGraphicsLineItem(0,0,0,0);
  this->y_zero->setPen(zero_pen);
  this->y_zero->setVisible(false);

  // Holds the graphs:
  this->graph_group = new GraphGroup();

  // Construct measure:
  this->_measure = new MeasureItem(this);
  this->_measure->setVisible(false);
  this->_measure->setPos(0,0);

  this->addToGroup(this->graph_group);
  this->addToGroup(this->x_zero);
  this->addToGroup(this->y_zero);
  addToGroup(_measure);
}


void
Axis::addGraph(Graph *graph)
{
  this->graphs.append(graph);
  this->graph_group->addToGroup(graph);
}


void
Axis::setXLabel(const QString &label)
{
  this->xticks->setLabel(label);
  this->updatePlotSize();
}


void
Axis::setYLabel(const QString &label)
{
  this->yticks->setLabel(label);
  this->updatePlotSize();
}


void
Axis::setXTicks(AxisTicks *ticks)
{
  this->removeFromGroup(this->xticks);
  delete this->xticks;
  this->xticks = ticks;
  this->addToGroup(this->xticks);
  this->updatePlotSize();
}


void
Axis::setYTicks(AxisTicks *ticks)
{
  this->removeFromGroup(this->yticks);
  delete this->yticks;
  this->yticks = ticks;
  this->addToGroup(this->yticks);
  this->updatePlotSize();
}


void
Axis::showXZero(bool show)
{
  this->x_zero->setVisible(show);
}


void
Axis::showYZero(bool show)
{
  this->y_zero->setVisible(show);
}


size_t
Axis::getNumGraphs()
{
  return this->graphs.size();
}


void
Axis::setAxisSize(const QSizeF &size)
{
  this->axis_size = size;
  this->updatePlotSize();
}


void
Axis::setXRange(const Range &range) {
  setXRange(range.min(), range.max());
}

void
Axis::setXRange(double min, double max)
{
  Extent old_extent = this->mapping->plotRange();
  Extent extent(min, max, old_extent.minY(), old_extent.maxY());
  this->mapping->setPlotRange(extent);
}


Range
Axis::getXRange()
{
  Extent extent = this->mapping->plotRange();
  return Range(extent.minX(), extent.maxX());
}


void
Axis::setXRangePolicy(const RangePolicy &policy)
{
  this->mapping->setXRangePolicy(policy);
}


RangePolicy
Axis::getXRangePolicy()
{
  return this->mapping->getXRangePolicy();
}


Range
Axis::getYRange()
{
  Extent extent = this->mapping->plotRange();
  return Range(extent.minY(), extent.maxY());
}

void
Axis::setYRange(const Range &range) {
  setYRange(range.min(), range.max());
}

void
Axis::setYRange(double min, double max)
{
  Extent old_extent = this->mapping->plotRange();
  Extent extent(old_extent.minX(), old_extent.maxX(), min, max);
  this->mapping->setPlotRange(extent);
}


void
Axis::setYRangePolicy(const RangePolicy &policy)
{
  this->mapping->setYRangePolicy(policy);
}


RangePolicy
Axis::getYRangePolicy()
{
  return this->mapping->getYRangePolicy();
}


void
Axis::setXMapFunction(MapFunction *func)
{
  this->mapping->setXMapFunction(func);
}


void
Axis::setYMapFunction(MapFunction *func)
{
  this->mapping->setYMapFunction(func);
}


Mapping *
Axis::getMapping() {
  return this->mapping;
}

QRectF
Axis::getPlotArea() const {
  return QRectF(this->yticks->boundingRect().width(), 0,
                this->mapping->size().width(), this->mapping->size().height());
}


void
Axis::showMeasure(const QPointF &point)
{
  // Skip if not in plot region disbale measure:
  if (! graph_group->boundingRect().contains(point-graph_group->pos())) {
    _measure->setVisible(false);
    return;
  }

  // Get coordinates of point:
  QPointF values = mapping->inverseMapping(point-graph_group->pos());

  // Show measure:
  _measure->setValues(values);
  _measure->setPos(point);
  _measure->setVisible(true);
}


void
Axis::updatePlotSize()
{
  this->mapping->setSize(
        QSizeF(
          this->axis_size.width() - this->yticks->boundingRect().width(),
          this->axis_size.height() - this->xticks->boundingRect().height()));

  this->yticks->setPos(0,0);
  this->yticks->updateSize();

  this->xticks->setPos(this->yticks->boundingRect().width(),
                       this->axis_size.height()-this->xticks->boundingRect().height());
  this->xticks->updateSize();

  this->graph_group->setPos(this->yticks->boundingRect().width(), 0);
  this->graph_group->setClipping(
        QRectF(0, 0,
               this->axis_size.width() - this->yticks->boundingRect().width(),
               this->axis_size.height() - this->xticks->boundingRect().height()));
}


void
Axis::updatePlotRange()
{
  // Determine minimum plot-range needed to display all graphs:
  Extent extent; QList<Graph *>::iterator iter = this->graphs.begin();
  if (this->graphs.end() != iter) {
    extent = (*iter)->getExtent();
    iter++;
  }

  for (; iter != this->graphs.end(); iter++) {
    extent = extent.united((*iter)->getExtent());
  }

  // Set new plot range:
  this->mapping->updatePlotRange(extent);
  this->xticks->updateRange();
  this->yticks->updateRange();
}


void
Axis::forceRedraw()
{
  // Update position of x-zero line:
  QPointF a = (*(this->mapping))(QPointF(this->mapping->plotRange().minX(),0));
  QPointF b = (*(this->mapping))(QPointF(this->mapping->plotRange().maxX(),0));
  this->x_zero->setLine(a.x(), a.y(), b.x(), b.y());
  this->x_zero->setPos(this->yticks->boundingRect().width(),0);

  // Update position of y-zero line:
  a = (*(this->mapping))(QPointF(0, this->mapping->plotRange().minY()));
  b = (*(this->mapping))(QPointF(0, this->mapping->plotRange().maxY()));
  this->y_zero->setLine(a.x(), a.y(), b.x(), b.y());
  this->y_zero->setPos(this->yticks->boundingRect().width(),0);

  /// \todo Redraw of AxisTicks needed?

  // Signal all graphs to redraw:
  for (QList<Graph *>::iterator iter = this->graphs.begin(); iter != this->graphs.end(); iter++)
  {
    (*iter)->updateGraph(*(this->mapping));
  }

  /// @todo Redraw measure if visible

  /// \todo QT Redraw event needed?
}


void
Axis::setScheme(Configuration::Scheme scheme)
{
  // First, reset scheme for ticks:
  this->xticks->setScheme(scheme);
  this->yticks->setScheme(scheme);

  // Apply scheme on x&y-zero lines:
  QPen zero_pen(Configuration::getConfig()->getScheme(Configuration::DISPLAY_SCHEME).defaultPen());
  zero_pen.setColor(QColor(128, 128, 128, 64));
  this->x_zero->setPen(zero_pen);
  this->y_zero->setPen(zero_pen);

  // Apply scheme on graphs:
  for(QList<Graph *>::iterator iter=this->graphs.begin(); iter!=this->graphs.end(); iter++) {
    (*iter)->setScheme(scheme);
  }
}
