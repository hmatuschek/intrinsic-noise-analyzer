#ifndef __INA_APP_VIEWS_MODELVIEW_HH__
#define __INA_APP_VIEWS_MODELVIEW_HH__

#include <QWidget>
#include "../doctree/modelitem.hh"


/** Displays a short summary of a model. */
class ModelView : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit ModelView(ModelItem *model_item, QWidget *parent = 0);

private:
  /** Holds a weak reference to the model instance. */
  iNA::Ast::Model *_model;
};

#endif // MODELVIEW_HH
