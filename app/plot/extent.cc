#include "extent.hh"

using namespace Plot;


Extent::Extent()
  : min_x(0), max_x(0), min_y(0), max_y(0)
{
  // Pass...
}


Extent::Extent(double min_x, double max_x, double min_y, double max_y)
  : min_x(min_x), max_x(max_x), min_y(min_y), max_y(max_y)
{
  // Pass...
}


Extent::Extent(const Extent &other)
  : min_x(other.min_x), max_x(other.max_x), min_y(other.min_y), max_y(other.max_y)
{
  // Pass...
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
  return Extent(std::min(this->min_x, other.min_x),
                std::max(this->max_x, other.max_x),
                std::min(this->min_y, other.min_y),
                std::max(this->max_y, other.max_y));
}
