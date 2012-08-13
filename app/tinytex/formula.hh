#ifndef __INA_APP_TINYTEX_FORMULA_HH__
#define __INA_APP_TINYTEX_FORMULA_HH__

#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>


/** A couple of settings for the rendering. */
class MathContext {
public:
  /** Default context. */
  MathContext();
  /** copy constructor. */
  MathContext(const MathContext &other);

  qreal fontSize() const;
  void setFontSize(qreal size);

  int lineWidth() const;

protected:
  /** Holds the base-font of the context. */
  QFont _font;
};


/** A class describing the metrics of a MathFormulaItem.
 * This class is essential for the layout of the formula, as it describes important measures
 * of a formula item. */
class MathMetrics {
public:
  /** Empty contructor. */
  MathMetrics();
  /** Copy constructor. */
  MathMetrics(const MathMetrics &other);

  inline const QSizeF &size() const { return _size; }
  inline qreal width() const { return _size.width(); }
  inline qreal height() const { return _size.height(); }
  inline void setSize(const QSizeF &size) { _size=size; }
  inline void setWidth(qreal width) { _size.setWidth(width); }
  inline void setHeight(qreal height) { _size.setHeight(height); }

  inline const QSizeF &bbSize() const { return _bb_size; }
  inline qreal bbWidth() const { return _bb_size.width(); }
  inline qreal bbHeight() const { return _bb_size.height(); }
  inline void setBBSize(const QSizeF &size) { _bb_size=size; }
  inline void setBBWidth(qreal width) { _bb_size.setWidth(width); }
  inline void setBBHeight(qreal height) { _bb_size.setHeight(height); }

protected:
  /** Tight size of the item. */
  QSizeF _size;
  /** Bounding box of the item. */
  QSizeF _bb_size;
};


/** Just the base class of all math items. */
class MathFormulaItem {
public:
  /** Default constructor. */
  MathFormulaItem();

  /** To ensure this class is virtual. */
  virtual ~MathFormulaItem();

  /** Layouts the element and renders it into a graphics item and also updates the
   * metrics of the item. */
  virtual QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent=0) = 0;

  /** Returns the metrics of the item. */
  const MathMetrics &metrics() const;

protected:
  /** Holds the metrics of the item. */
  MathMetrics _metrics;
};


/** A container, renders as a series of formula items. */
class MathFormula : public MathFormulaItem
{
public:
  MathFormula();
  virtual ~MathFormula();

  size_t size() const;
  void appendItem(MathFormulaItem *item);
  void prependItem(MathFormulaItem *item);

  QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent=0);

private:
  QList<MathFormulaItem *> _items;
};


/** Simple formula item to draw a fraction. */
class MathFraction : public MathFormulaItem
{
public:
  MathFraction(MathFormulaItem *nom, MathFormulaItem *denom);
  virtual ~MathFraction();
  virtual QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);

private:
  MathFormulaItem *_nominator;
  MathFormulaItem *_denominator;
};


/** Simple pain text element. */
class MathText : public MathFormulaItem
{
public:
  MathText(const QString &text);
  virtual ~MathText();
  QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);

private:
  QString _text;
};

/** Simple X^Y formula element. */
class MathSup : public MathFormulaItem
{
public:
  MathSup(MathFormulaItem *base, MathFormulaItem *upper);
  virtual ~MathSup();
  QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);

private:
  MathFormulaItem *_base;
  MathFormulaItem *_upper;
};


class MathSub : public MathFormulaItem, public QGraphicsItemGroup
{
public:
  MathSub(MathFormulaItem *base, MathFormulaItem *lower);
  virtual ~MathSub();
  QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent);

private:
  MathFormulaItem *_base;
  MathFormulaItem *_lower;
};

#endif // __INA_APP_TINYTEX_FORMULA_HH__
