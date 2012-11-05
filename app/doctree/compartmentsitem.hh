#ifndef SBMLMODELCOMPARTMENTS_HH
#define SBMLMODELCOMPARTMENTS_HH


#include <QObject>
#include "documenttreeitem.hh"
#include "../models/compartmentlist.hh"
#include "ast/model.hh"

class Application;


/**
 * Represents the list of model compartments at the same time it implements the TreeItem interface
 * to act as a leaf of the model tree.
 *
 * @ingroup gui
 */
class CompartmentsItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

protected:
  /**
   * Holds the CompartmentList model (has the ownership).
   */
  CompartmentList *_compartments;

  /**
   * The label shown in the document tree.
   */
  QString itemLabel;


public:
  /**
   * Constructs a new "Compartments" item for the DocumentTree.
   */
  CompartmentsItem(iNA::Ast::Model *model, QObject *parent=0);

  /**
   * Implemente @c Wrapper interface.
   */
  virtual const QString & getLabel() const;

  /**
   * Returns a weak reference to the list of compartments.
   */
  CompartmentList *compartments();

  /**
   * Retrurns true.
   */
  virtual bool providesView() const;

  /**
   * Returns a CompartmentView instance.
   */
  virtual QWidget *createView();
};


#endif // SBMLMODELCOMPARTMENTS_HH
