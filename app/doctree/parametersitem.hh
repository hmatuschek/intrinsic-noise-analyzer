#ifndef SBMLMODELPARAMETERS_HH
#define SBMLMODELPARAMETERS_HH

#include <QObject>
#include "ast/model.hh"
#include "documenttreeitem.hh"
#include "../models/parameterlist.hh"


class Application;


/** Represents the list of model parameters at the same time it implements the TreeItem interface
 * to act as a leaf of the model tree.
 * @ingroup gui */
class ParametersItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

public:
  /** Constructs a new parameter list item for the given model. */
  ParametersItem(iNA::Ast::Model *model, QObject *parent=0);

  /** Implementation of the @c TreeItem interface. */
  virtual const QString &getLabel() const;

  /** Returns true. */
  virtual bool providesView() const;

  /** Returns an instance of @c ParameterView showing the list of parameters. */
  virtual QWidget *createView();

protected:
  /** Holds the display label of this item in the @c DocumentTree. */
  QString _itemLabel;
};



#endif // SBMLMODELPARAMETERS_HH
