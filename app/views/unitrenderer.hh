#ifndef __INA_APP_VIEWS_UNITRENDERER_HH__
#define __INA_APP_VIEWS_UNITRENDERER_HH__

#include <QPixmap>
#include <QList>
#include "../tinytex/custommula.hh"

#include "ast/unitdefinition.hh"
#include <QGraphicsScene>


/** A renderer custom units. */
class UnitRenderer : QObject
{
  Q_OBJECT

public:
  explicit UnitRenderer(const iNA::Ast::Unit &unit, QObject *parent = 0);
  virtual ~UnitRenderer();

  const QPixmap &toPixmap();

public:
  static QPixmap toPixmap(const iNA::Ast::Unit &unit);

private:
  Mathcustommula *_custommula;
  QPixmap *_pixmap;
};

#endif // UNITRENDERER_HH
