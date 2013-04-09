#ifndef __FLUC_PLOT_MAPPING_HH__
#define __FLUC_PLOT_MAPPING_HH__

#include <QSizeF>
#include <QPointF>
#include <QObject>
#include <QPainterPath>

#include "extent.hh"
#include <eigen3/Eigen/Eigen>


namespace Plot {


/** Provides a 1D range/interval [min, max].
 * @ingroup plot */
class Range
{
public:
  /** Costructs a range from given minimum and maximum. */
  Range(double min, double max);
  /** Copy constructor. */
  Range(const Range &other);

  /** Returns the minimum of the interval. */
  inline double min() const { return this->_min; }
  /** Sets the minimum of the interval. */
  inline void setMin(double value) { this->_min = value; }
  /** Returns the maximum of the interval. */
  inline double max() const { return this->_max; }
  /** Sets the maximum of the interval. */
  inline void setMax(double value) { this->_max = value; }
  /** Returns the interval size (maximum - minimum). */
  inline double delta() const { return this->_max - this->_min; }

protected:
  /** Holds the minimum value of the interval. */
  double _min;
  /** Holds the maximum value of the interval. */
  double _max;
};



/**
 * This class describes the plot-range-policy.
 *
 * This policy describes how the plot-range (@c Range) of an axis is determined.
 *
 * @ingroup plot
 */
class RangePolicy
{
public:
  /**
   * Defines the available plot-range-policies.
   */
  typedef enum {
    FIXED,      /// < Specifies that an axis is fixed in a direction.
    AUTOMATIC   /// < Specifies that the extent of an axis in a direction is determined automatically.
  } Policy;


protected:
  /**
   * Holds the policy for the range-minimum.
   */
  Policy min_policy;

  /**
   * Holds the policy for the range-maximum.
   */
  Policy max_policy;


public:
  /**
   * Constructs a range-policy, using the same (given) policy for the minimum and the maximum.
   */
  RangePolicy(Policy policy);

  /**
   * Constructs a range-policy from the given policies.
   */
  RangePolicy(Policy min_policy, Policy max_policy);

  /**
   * Returns the policy for the range-minimum.
   */
  Policy getMinPolicy() const;

  /**
   * Returns the policy for the range-maximum.
   */
  Policy getMaxPolicy() const;

  /**
   * (Re-) Sets the policy for the range-minimum.
   */
  void setMinPolicy(Policy policy);

  /**
   * (Re-) Sets the policy for the range-maximum.
   */
  void setMaxPolicy(Policy policy);
};



/**
 * Simple base class for an unary map-function used by @c Plot::Mapping.
 *
 * Maps an interval specified by range into the interval [0, 1].
 *
 * @ingroup plot
 */
class MapFunction : public QObject
{
  Q_OBJECT

protected:
  /**
   * Holds the interval being mapped to [0, 1].
   */
  Range _range;

  /**
   * Holds the range-policy for the mapping.
   *
   * The map-function uses this policies to update the plot-range if needed.
   */
  RangePolicy _policy;


public:
  /**
   * Constructs the mapping from the given interval to [0, 1].
   */
  explicit MapFunction(const Range &range, const RangePolicy &policy, QObject *parent=0);

  /**
   * Returns the interval being mapped.
   */
  const Range &range() const;

  /**
   * Resets the interval being mapped.
   */
  void setRange(const Range &range);

  /**
   * Returns the plot-range-policy.
   */
  const RangePolicy &policy() const;

  /**
   * (Re-) Sets the plot-range-policy.
   */
  void setPolicy(const RangePolicy &policy);

  /**
   * Soft update of the range according to the specified policy.
   */
  virtual void updateRange(const Range &range) = 0;

  /**
   * Needs to be overridden to implement the actual mapping.
   */
  virtual double operator() (double x) const = 0;

  /**
   * Needs to be overridden to implement a sampling from the _range interval, so that the mapping
   * of the sampled values is uniform.
   */
  virtual void sample(Eigen::VectorXd &samples) const = 0;

  /** Performs the inverse mapping from [0,1] into the plot range. */
  virtual double inverseMap(const double &value) const = 0;
};



/**
 * Implements a linear mapping (scaleing).
 *
 * @ingroup plot
 */
class LinearMapFunction : public MapFunction
{
  Q_OBJECT

public:
  /**
   * Constructs a linear map function from the given interval to [0, 1].
   */
  explicit LinearMapFunction(const Range &range, const RangePolicy &policy, QObject *parent=0);

  /**
   * Updates the plot-range according to the setted plot-range-policy.
   *
   * @see @c setPolicy.
   */
  virtual void updateRange(const Range &range);

  /**
   * Implements the linear mapping.
   */
  virtual double operator() (double x) const;

  /** Samples equally from the source interval. */
  virtual void sample(Eigen::VectorXd &samples) const;

  /** Performs the inverse mapping. */
  virtual double inverseMap(const double &value) const;
};



/**
 * Implements a logarithmic mapping function.
 *
 * @ingroup plot
 */
class LogMapFunction : public MapFunction
{
  Q_OBJECT

public:
  /**
   * Constructs the logarithmic mapping for the given range.
   */
  explicit LogMapFunction(const Range &range, const RangePolicy &policy,  QObject *parent=0);

  /**
   * Updates the plot-range according to the setted plot-range-policy.
   *
   * @see @c setPolicy.
   */
  virtual void updateRange(const Range &range);

  /**
   * Performs the mapping.
   */
  virtual double operator() (double x) const;

  /**
   * Samples logarithmic from _range.
   */
  virtual void sample(Eigen::VectorXd &samples) const;

  /** Performs the inverse mapping. */
  virtual double inverseMap(const double &value) const;
};



/**
 * Implements the mapping function to map a point into an area of @c plot_size.
 *
 * @ingroup plot
 */
class Mapping : public QObject
{
  Q_OBJECT

protected:
  /**
   * Holds the size (width, height) of the plot-area.
   */
  QSizeF plot_size;

  /**
   * Holds the mapping for the x-axis.
   */
  MapFunction *x_map;

  /**
   * Holds the mapping for the y-axis.
   */
  MapFunction *y_map;


public:
  /**
   * Constructs a default (linear) mapping from the given plot-ranges to the given plot-size.
   */
  Mapping(const Extent &plot_range, const QSizeF &size, QObject *parent=0);

  /**
   * Returns the current plot-size.
   */
  const QSizeF &size() const;

  /**
   * (Re-) Sets the current plot-size.
   */
  void setSize(const QSizeF &size);

  /**
   * Retunrs the current plot-ranges.
   */
  Extent plotRange() const;

  /**
   * (Re-) Sets the current plot-ranges.
   */
  void setPlotRange(const Extent &range);

  /**
   * Updates the plot-ranges.
   */
  void updatePlotRange(const Extent &range);

  /**
   * (Re-) Sets the current plot-range-policies for the x-axis.
   */
  void setXRangePolicy(const RangePolicy &policy);

  /**
   * Returns the current plot-range-policies for the x-axis.
   */
  RangePolicy getXRangePolicy();

  /**
   * (Re-) Sets the current plot-range-policies for the y-axis.
   */
  void setYRangePolicy(const RangePolicy &policy);

  /**
   * Returns the current plot-range-policies for the y-axis.
   */
  RangePolicy getYRangePolicy();

  /**
   * Resets the map function for the X axis.
   */
  void setXMapFunction(MapFunction *func);

  /**
   * Resets the map function for the Y axis.
   */
  void setYMapFunction(MapFunction *func);

  /**
   * Maps a given point to parent coordinates.
   */
  virtual QPointF operator() (const QPointF &point) const;

  /**
   * Maps a given path to parent coordinates.
   */
  virtual QPainterPath operator() (const QPainterPath &path) const;

  /** Maps parent coordinates to plot-coordinates. */
  virtual QPointF inverseMapping(const QPointF &point) const;

  /**
   * Samples the x-axis.
   */
  virtual void sampleX(Eigen::VectorXd &samples) const;

  /**
   * Samples the y-axis.
   */
  virtual void sampleY(Eigen::VectorXd &samples) const;
};


}

#endif // MAPPING_HH
