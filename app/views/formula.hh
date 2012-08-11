#ifndef __INA_APP_VIEWS_FORMULA_HH__
#define __INA_APP_VIEWS_FORMULA_HH__

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

  /** The current font-size. */
  int fontSize;
};


/** A class describing the metrics of a MathFormulaItem. */
class MathMetrics {
public:
  MathMetrics();
  MathMetrics(const MathMetrics &other);
  double width;
  double height;
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

  void addItem(MathFormulaItem *item);

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


class MathText : public MathFormulaItem
{
public:
  MathText(const QString &text);
  virtual ~MathText();
  QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);

private:
  QString _text;
};


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


/*class MathSub : public MathFormulaItem, public QGraphicsItemGroup
{
public:
  MathSub();

private:
  MathFormulaItem *_base;
  MathFormulaItem *_lower;
};*/

#endif // __INA_APP_VIEWS_FORMULA_HH__
