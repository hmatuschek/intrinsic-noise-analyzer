#include "mapping.hh"
#include <iostream>

using namespace Plot;

/**
* Helper that rounds to n leading digits
*/
double roundRange(double num, int n) {
    if(num == 0) {
        return 0;
    }

    double d = std::ceil(std::log10(num < 0 ? -num: num));
    int power = n - (int) d;

    double magnitude = std::pow(10.0, power);
    long shifted = std::floor(num*magnitude+0.5);
    return shifted/magnitude;
}


/* ********************************************************************************************* *
 * Implementation of base Range:
 * ********************************************************************************************* */
Range::Range(double min, double max)
  : _min(min), _max(max)
{
  // Pass...
}


Range::Range(const Range &other)
  : _min(other._min), _max(other._max)
{
  // Pass...
}



/* ********************************************************************************************* *
 * Implementation of base RangePolicy:
 * ********************************************************************************************* */
RangePolicy::RangePolicy(Policy policy)
  : min_policy(policy), max_policy(policy)
{
  // Pass...
}


RangePolicy::RangePolicy(Policy min_policy, Policy max_policy)
  : min_policy(min_policy), max_policy(max_policy)
{
  // Pass...
}


RangePolicy::Policy
RangePolicy::getMinPolicy() const
{
  return this->min_policy;
}


RangePolicy::Policy
RangePolicy::getMaxPolicy() const
{
  return this->max_policy;
}


void
RangePolicy::setMinPolicy(RangePolicy::Policy policy)
{
  this->min_policy = policy;
}


void
RangePolicy::setMaxPolicy(Policy policy)
{
  this->max_policy = policy;
}



/* ********************************************************************************************* *
 * Implementation of base map-functional:
 * ********************************************************************************************* */
MapFunction::MapFunction(const Range &range, const RangePolicy &policy, QObject *parent)
  : QObject(parent), _range(range), _policy(policy)
{
  // pass...
}

const Range &
MapFunction::range() const
{
  return this->_range;
}

void
MapFunction::setRange(const Range &range)
{
  this->_range = range;
}


const RangePolicy &
MapFunction::policy() const
{
  return this->_policy;
}


void
MapFunction::setPolicy(const RangePolicy &policy)
{
  this->_policy = policy;
}



/* ********************************************************************************************* *
 * Implementation of linear mapping.
 * ********************************************************************************************* */
LinearMapFunction::LinearMapFunction(const Range &range, const RangePolicy &policy, QObject *parent)
  : MapFunction(range, policy, parent)
{
  // Pass...
}

void
LinearMapFunction::updateRange(const Range &range)
{
  // Round to pecision of 2
  double rmin = roundRange(range.min(),2);
  double rmax = roundRange(range.max(),2);

  if (RangePolicy::FIXED == this->_policy.getMinPolicy())
  {
    // Keep old value:
    rmin = this->range().min();
  }

  if (RangePolicy::FIXED == this->_policy.getMaxPolicy())
  {
    // Keep old value:
    rmax = this->range().max();
  }

  this->setRange(Range(rmin, rmax));
}


double
LinearMapFunction::operator ()(double x) const
{
  return (x-this->range().min())/this->range().delta();
}


void
LinearMapFunction::sample(Eigen::VectorXd &samples) const
{
  size_t N = samples.size();
  double dx = this->range().delta()/(N-1);

  if (0 == N)
  {
    return;
  }

  samples(0) = this->range().min();
  for (size_t i=1; i<N; i++)
  {
    samples(i) = samples(i-1) + dx;
  }
}



/* ********************************************************************************************* *
 * Implementation of log mapping.
 * ********************************************************************************************* */
LogMapFunction::LogMapFunction(const Range &range, const RangePolicy &policy, QObject *parent)
  : MapFunction(range, policy, parent)
{
  // Pass...
}

void
LogMapFunction::updateRange(const Range &range)
{
  this->setRange(range);
}


double
LogMapFunction::operator ()(double x) const
{
  return std::log10(1. + 9.*(x - this->range().min())/this->range().delta());
}


void
LogMapFunction::sample(Eigen::VectorXd &samples) const
{
  size_t N = 0;
  if (0 == (N = samples.size()))
  {
    return;
  }

  double dx = std::exp(std::log(this->range().delta())/(N-1));

  samples(0) = this->range().min(); double offset = 1.0;
  for (size_t i=1; i<N; i++)
  {
    offset = offset * dx;
    samples(i) = this->range().min() + offset;
  }
}



/* ********************************************************************************************* *
 * Implementation of Coordinate system mapping:
 * ********************************************************************************************* */
Mapping::Mapping(const Extent &plot_range, const QSizeF &size, QObject *parent)
  : QObject(parent), plot_size(size)
{
  this->setXMapFunction(new LinearMapFunction(Range(plot_range.minX(), plot_range.maxX()),
                                              RangePolicy(RangePolicy::AUTOMATIC)));
  this->setYMapFunction(new LinearMapFunction(Range(plot_range.minY(), plot_range.maxY()),
                                              RangePolicy(RangePolicy::AUTOMATIC)));
}


Extent
Mapping::plotRange() const
{
  Extent range(this->x_map->range().min(), this->x_map->range().max(),
               this->y_map->range().min(), this->y_map->range().max());
  return range;
}


const QSizeF &
Mapping::size() const
{
  return this->plot_size;
}


void
Mapping::setSize(const QSizeF &size)
{
  this->plot_size = size;
}


void
Mapping::setPlotRange(const Extent &range)
{
  this->x_map->setRange(Range(range.minX(), range.maxX()));
  this->y_map->setRange(Range(range.minY(), range.maxY()));
}


void
Mapping::updatePlotRange(const Extent &range)
{
  this->x_map->updateRange(Range(range.minX(), range.maxX()));
  this->y_map->updateRange(Range(range.minY(), range.maxY()));
}


RangePolicy
Mapping::getXRangePolicy()
{
  return this->x_map->policy();
}


void
Mapping::setXRangePolicy(const RangePolicy &policy)
{
  this->x_map->setPolicy(policy);
}


RangePolicy
Mapping::getYRangePolicy()
{
  return this->y_map->policy();
}


void
Mapping::setYRangePolicy(const RangePolicy &policy)
{
  this->y_map->setPolicy(policy);
}


void
Mapping::setXMapFunction(MapFunction *func)
{
  this->x_map = func;
  func->setParent(this);
}


void
Mapping::setYMapFunction(MapFunction *func)
{
  this->y_map = func;
  func->setParent(this);
}


QPointF
Mapping::operator ()(const QPointF &point) const
{
  return QPointF(
        this->plot_size.width()*(*(this->x_map))(point.x()),
        this->plot_size.height()*(1.-(*(this->y_map))(point.y())));
}


QPainterPath
Mapping::operator ()(const QPainterPath &path) const
{
  QPainterPath new_path;

  for (int i=0; i<path.elementCount(); i++)
  {
    const QPainterPath::Element &elm = path.elementAt(i);

    if (elm.isMoveTo()) {
      QPointF dest(elm.x, elm.y);
      dest = (*this)(dest);
      new_path.moveTo(dest.x(), dest.y());
    }
    else if (elm.isLineTo())
    {
      QPointF dest(elm.x, elm.y);
      dest = (*this)(dest);
      new_path.lineTo(dest.x(), dest.y());
    }
  }

  return new_path;
}


void
Mapping::sampleX(Eigen::VectorXd &samples) const
{
  this->x_map->sample(samples);
}


void
Mapping::sampleY(Eigen::VectorXd &samples) const
{
  this->y_map->sample(samples);
}
