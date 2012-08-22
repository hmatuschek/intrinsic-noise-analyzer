#ifndef __INA_APP_VIEWS_UNITRENDERER_HH__
#define __INA_APP_VIEWS_UNITRENDERER_HH__

#include <QPixmap>
#include <QList>
#include "../tinytex/formula.hh"

#include "ast/unitdefinition.hh"
#include <QGraphicsScene>


/** A renderer for units. */
class UnitRenderer : QObject
{
  Q_OBJECT

public:
  explicit UnitRenderer(const Fluc::Ast::Unit &unit, QObject *parent = 0);
  virtual ~UnitRenderer();

  const QPixmap &toPixmap();

private:
  MathFormula *_formula;
  QPixmap *_pixmap;
};

#endif // UNITRENDERER_HH
