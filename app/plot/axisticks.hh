#ifndef __PLOT_AXISTICKS_HH__
#define __PLOT_AXISTICKS_HH__

#include <QVector>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsLineItem>

#include "mapping.hh"
#include "configuration.hh"
#include "../tinytex/tinytex.hh"



namespace Plot {


/**
 * Represents a single axis tick (small dash with some text/value).
 *
 * All axis ticks have a value, determining the position of the tick along the axis. An axis tick
 * may also have a label, if present, it is shown instead of the value.
 *
 * @ingroup plot
 */
class AxisTick : public QGraphicsItemGroup
{
public:
  /**
   * Speciefies the position of the tick.
   */
  typedef enum
  {
    LEFT,    ///< The tick belongs to an axis, displayed on the left side of the graph.
    RIGHT,   ///< The tick belongs to an axis, displayed on the right side of the graph.
    TOP,     ///< The tick belongs to an axis, displayed above the graph.
    BOTTOM   ///< The tick belongs to an axis, displayed below the graph.
  } Orientation;


protected:
  /**
   * Holds the value of the tick, determining the position of the tick and the label, if no
   * label has been assigned to the tick.
   */
  double value;

  /**
   * Holds the graphics item for the label.
   */
  QGraphicsTextItem *label;

  /**
   * Holds the graphics item for the tick (short line).
   */
  QGraphicsLineItem *tick;

  /**
   * Holds the orientation of the tick.
   */
  Orientation orientation;


public:
  /**
   * Constructs a new axis tick for the given value and orientation.
   */
  AxisTick(double value, Orientation orientation);

  /**
   * Constructs a new axis tick with the given value, label (displayed instead of the value) and
   * orientation.
   */
  AxisTick(double value, const QString &label, Orientation orientation);

  /**
   * Resets the label of the tick.
   */
  void setLabel(const QString &label);

  /**
   * Returns the label as a string.
   */
  QString getLabel();

  /**
   * Returns the value of the tick.
   */
  double getValue() const;

  /**
   * Resets the value of the tick.
   */
  void setValue(double value, bool update_label);

  /**
   * Returns the bounding box of the complete item (label and tick).
   */
  virtual QRectF boundingRect() const;


public slots:
  /**
   * (Re-) Sets the plot-scheme.
   */
  void setScheme(Configuration::Scheme scheme);
};



/** Represents a single axis with ticks and label.
 * @ingroup plot */
class AxisTicks : public QGraphicsItemGroup
{
public:
  /** Constructs a new axis. */
  AxisTicks(Mapping *_mapping, AxisTick::Orientation _orientation, const QString &label = "");

  /** (Re-) Sets the label. */
  void setLabel(const QString &label);

  /** Adds a tick. */
  void addTick(AxisTick *tick);

  /** Returns the bounding-box. */
  virtual QRectF boundingRect() const;


public slots:
  /** Updates the ranges, rearanges ticks. */
  void updateRange();

  /** Updates the display-size, rearanges ticks. */
  void updateSize();

  /** (Re-) Sets the plot-scheme. */
  void setScheme(Configuration::Scheme scheme);


protected:
  /** Updates the labels. */
  virtual void updateLables();
  /** Updates all positions (including positions of ticks). */
  virtual void updatePositions();

protected:
  /** The orientation. */
  AxisTick::Orientation _orientation;
  /** The Ticks. */
  QVector<AxisTick *> _ticks;
  /** The axis line. */
  QGraphicsLineItem *_axis_line;
  /** The mapping of the axis. */
  Mapping *_mapping;
  /** Holds the math context for the axis label rendering. */
  MathContext _mathcontext;
  /** Holds the axis label text. */
  QString _labeltext;
  /** Holds the extend of the axis. */
  QSizeF _labelsize;
  /** The axis label as graphics element. */
  QGraphicsItem *_labelitem;
};



/** Default AxisTicks class.
 * @ingroup plot */
class AutoAxisTicks : public AxisTicks
{
public:
  /** Constructs axis. */
  AutoAxisTicks(Mapping *_mapping, size_t num_ticks, AxisTick::Orientation _orientation, const QString &label = "");

protected:
  /** Determines tick labels from values. */
  virtual void updateLables();
};


}

#endif // __PLOT_AXISTICKS_HH__
