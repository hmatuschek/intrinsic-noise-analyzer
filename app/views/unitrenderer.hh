#ifndef __INA_APP_VIEWS_UNITRENDERER_HH__
#define __INA_APP_VIEWS_UNITRENDERER_HH__

#include <QPixmap>
#include <QList>
#include "../tinytex/formula.hh"

#include "ast/unitdefinition.hh"
#include <QGraphicsScene>


/** A renderer for units. */
class UnitRenderer : public QObject
{
  Q_OBJECT

public:
  /** Constructor, prerenders the unit. */
  explicit UnitRenderer(const iNA::Ast::Unit &unit, QObject *parent = 0);
  /** Destructor. */
  virtual ~UnitRenderer();

  /** (Re-) Sets the unit. */
  void setUnit(const iNA::Ast::Unit &unit);

  /** Renders the unit as a pixmap. */
  const QPixmap &toPixmap();
  /** Renders the unit on the painter. */
  void renderOn(QPainter *painter, const QRect &target=QRect(), const QRect &source=QRect());
  /** Returns the required size. */
  QSize size() const;

public:
  /** Direct rendering on a pixmap. */
  static QPixmap toPixmap(const iNA::Ast::Unit &unit);
  /** Direct rendering on a painter. */
  static void renderOn(const iNA::Ast::Unit &unit, QPainter *painter);

private:
  /** Helper function to render the formula into a scene. */
  void _renderFormula();

private:
  /** Holds the unit as a formula. */
  MathFormula *_formula;
  /** Holds the rendered formula as a QGraphicsScene. */
  QGraphicsScene *_scene;
  /** Holds the rendered unit as a pixmap. */
  QPixmap *_pixmap;
};

#endif // _INA_APP_VIEWS_UNITRENDERER_HH__
