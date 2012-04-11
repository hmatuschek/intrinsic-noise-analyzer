#ifndef __PLOT_EXTENT_HH__
#define __PLOT_EXTENT_HH__

#include <cstdlib>
#include <algorithm>


namespace Plot {

/**
 * A class to handle x- and y-ranges.
 *
 * @ingroup plot
 */
class Extent
{
protected:
  /**
   * Holds the min. in x-direction.
   */
  double min_x;

  /**
   * Holds the max. in x-direction.
   */
  double max_x;

  /**
   * Holds the min. in y-direction.
   */
  double min_y;

  /**
   * Holds the max. in y-direction.
   */
  double max_y;


public:
  /**
   * Constructs an empty ((0,0),(0,0)) extent.
   */
  Extent();

  /**
   * Constructor with given extent.
   */
  Extent(double min_x, double max_x, double min_y, double max_y);

  /**
   * Copy constructor.
   */
  Extent(const Extent &other);

  /**
   * Assignment operator.
   */
  const Extent & operator =(const Extent &other);

  /**
   * Computes the union of this and the given extent.
   */
  Extent united(const Extent other) const;

  /**
   * Returns the min. in x-direction.
   */
  inline double minX() const { return this->min_x; }

  /**
   * Returns the max. in x-direction.
   */
  inline double maxX() const { return this->max_x; }

  /**
   * Returns the min. in y-direction.
   */
  inline double minY() const { return this->min_y; }

  /**
   * Returns the max. in y-direction.
   */
  inline double maxY() const { return this->max_y; }

  /**
   * Returns the x-interval.
   */
  inline double dX() const { return this->max_x - this->min_x; }

  /**
   * Returns the y-interval.
   */
  inline double dY() const { return this->max_y - this->min_y; }
};


}


#endif // EXTENT_HH
