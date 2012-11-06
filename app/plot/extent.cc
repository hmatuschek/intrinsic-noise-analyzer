#include "extent.hh"
#include <math.hh>

using namespace Plot;
using namespace iNA;


Extent::Extent()
  : min_x(0), max_x(0), min_y(0), max_y(0)
{
  // Pass...
}


Extent::Extent(double min_x, double max_x, double min_y, double max_y)
  : min_x(min_x), max_x(max_x), min_y(min_y), max_y(max_y)
{
  this->min_x = Math::isNotValue(this->min_x) ? 0 : this->min_x;
  this->max_x = Math::isNotValue(this->max_x) ? 0 : this->max_x;
  this->min_y = Math::isNotValue(this->min_y) ? 0 : this->min_y;
  this->max_y = Math::isNotValue(this->max_y) ? 0 : this->max_y;
}


Extent::Extent(const Extent &other)
  : min_x(other.min_x), max_x(other.max_x), min_y(other.min_y), max_y(other.max_y)
{
  min_x = Math::isNotValue(min_x) ? 0 : min_x;
  max_x = Math::isNotValue(max_x) ? 0 : max_x;
  min_y = Math::isNotValue(min_y) ? 0 : min_y;
  max_y = Math::isNotValue(max_y) ? 0 : max_y;
}


const Extent &
Extent::operator =(const Extent &other)
{
  this->min_x = other.min_x; this->max_x = other.max_x;
  this->min_y = other.min_y; this->max_y = other.max_y;

  return *this;
}


Extent
Extent::united(const Extent other) const
{
  double min_x = Math::min(this->min_x, other.min_x);
  double max_x = Math::max(this->min_x, other.min_x);
  double min_y = Math::min(this->min_y, other.min_y);
  double max_y = Math::max(this->min_y, other.min_y);

  return Extent(min_x, max_x, min_y, max_y);
}
