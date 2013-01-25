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
  /** Creates a context from the given font. */
  MathContext(const QFont &font);
  /** copy constructor. */
  MathContext(const MathContext &other);

  /** Returns the size of the current font in pixel. */
  qreal pixelSize() const;
  /** Returns the current fontsize. */
  qreal fontSize() const;
  /** Sets the current font size. */
  void setFontSize(qreal size);
  /** Sets the complete font for the context. */
  void setFont(const QFont &font);
  /** Fontsize dependent line width. */
  qreal lineWidth() const;

protected:
  /** Holds the base-font of the context. */
  QFont _font;
};


/** A the metrics of a MathFormulaItem.
 * This class is essential for the layout of the formula, as it describes important measures
 * of a formula item. */
class MathMetrics {
public:
  /** Empty contructor. */
  MathMetrics();
  /** Copy constructor. */
  MathMetrics(const MathMetrics &other);

  /** Returns the logical size of the item. */
  inline QSizeF size() const { return QSizeF(width(), height()); }
  /** Returns the logical width. */
  inline qreal width() const { return _width; }
  inline qreal leftBearing() const { return _left_bearing; }
  inline qreal rightBearing() const { return _right_bearing; }
  /** Returns the logical height = ascent() + descent(); */
  inline qreal height() const { return _height; }
  inline qreal ascent() const { return _ascent; }
  inline qreal descent() const { return _height-_ascent; }
  inline qreal center() const { return _center; }
  inline qreal tail() const { return _height-_center; }
  /** Sets the logical width of the item. */
  inline void setWidth(qreal width) { _width = width; }
  inline void setHeight(qreal value) { _height = value; }
  inline void setAscent(qreal value) { _ascent = value; }
  inline void setCenter(qreal value) { _center = value; }
  inline void setLeftBearing(qreal value) { _left_bearing = value; }
  inline void setRightBearing(qreal value) { _right_bearing = value; }

  inline const QRectF &bb() const { return _bbox; }
  inline QSizeF bbSize() const { return _bbox.size(); }
  inline qreal bbWidth() const { return _bbox.width(); }
  inline qreal bbHeight() const { return _bbox.height(); }
  inline qreal bbAscent() const { return ascent(); }
  inline void setBB(const QRectF &bb) { _bbox = bb; }

protected:
  qreal _height;
  qreal _width;
  qreal _ascent;
  qreal _center;
  qreal _left_bearing;
  qreal _right_bearing;
  QRectF _bbox;
};


/** Just the base class of all math items. */
class MathItem {
public:
  /** Default constructor. */
  MathItem();
  /** Copy constructor. */
  MathItem(const MathItem &other);

  /** To ensure this class is virtual. */
  virtual ~MathItem();

  /** Layouts the element and renders it into a graphics item and also updates the
   * metrics of the item. */
  virtual QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent=0) = 0;

  /** Copy method. */
  virtual MathItem *copy() const = 0;

  /** Returns the metrics of the item. */
  const MathMetrics &metrics() const;

  /** Direct rendering into a @c QPixmap. */
  QPixmap renderItem(const MathContext &ctx=MathContext());

protected:
  /** Holds the metrics of the item. */
  MathMetrics _metrics;
};


/** A container, renders as a series of formula items. */
class MathFormula : public MathItem
{
public:
  MathFormula();
  MathFormula(const MathFormula &other);

  virtual ~MathFormula();

  size_t size() const;
  void appendItem(MathItem *item);
  void prependItem(MathItem *item);

  QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent=0);
  MathItem *copy() const;

private:
  QList<MathItem *> _items;
};


/** Simple block with left and right delimiters. */
class MathBlock : public MathItem {
public:
  MathBlock(MathItem *center, MathItem *left=0, MathItem *right=0);
  MathBlock(const MathBlock &other);
  virtual ~MathBlock();

  QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent);
  MathItem *copy() const;

private:
  MathItem *_center;
  MathItem *_left;
  MathItem *_right;
};


/** Simple class to implement certain spaces. */
class MathSpace : public MathItem {
public:
  /** Some common spaces in TeX mathmode. */
  typedef enum {
    THIN_SPACE,    ///< "\,"
    MEDIUM_SPACE,  ///< "\:"
    THICK_SPACE,   ///< "\;"
    QUAD_SPACE     ///< "\quad"
  } TeXSpace;

public:
  MathSpace(TeXSpace tex_space);
  MathSpace(qreal factor);
  MathSpace(const MathSpace &other);
  virtual ~MathSpace();

  QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent);
  MathItem *copy() const;

private:
  /** The actual space factor. */
  qreal _factor;
};


/** Simple formula item to draw a fraction. */
class MathFraction : public MathItem
{
public:
  MathFraction(MathItem *nom, MathItem *denom);
  MathFraction(const MathFraction &other);
  virtual ~MathFraction();

  virtual QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);
  virtual MathItem *copy() const;

private:
  MathItem *_nominator;
  MathItem *_denominator;
};


/** Simple pain text element. */
class MathText : public MathItem
{
public:
  MathText(const QString &text);
  MathText(const MathText &other);
  virtual ~MathText();

  QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);
  MathItem *copy() const;

private:
  QString _text;
};


/** Simple symbol element, similar to MathText but with an different alignment. */
class MathSymbol : public MathItem
{
public:
  MathSymbol(QChar symbol);
  MathSymbol(const MathSymbol &symbol);
  virtual ~MathSymbol();

  QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent);
  MathItem *copy() const;

private:
  QChar _symbol;
};


/** Simple X^Y formula element. */
class MathSup : public MathItem
{
public:
  MathSup(MathItem *base, MathItem *upper);
  MathSup(const MathSup &other);
  virtual ~MathSup();

  QGraphicsItem* layout(const MathContext &context, QGraphicsItem *parent=0);
  MathItem *copy() const;

private:
  MathItem *_base;
  MathItem *_upper;
};


class MathSub : public MathItem, public QGraphicsItemGroup
{
public:
  MathSub(MathItem *base, MathItem *lower);
  MathSub(const MathSub &other);
  virtual ~MathSub();

  QGraphicsItem *layout(const MathContext &context, QGraphicsItem *parent);
  MathItem *copy() const;

private:
  MathItem *_base;
  MathItem *_lower;
};

#endif // __INA_APP_TINYTEX_FORMULA_HH__
