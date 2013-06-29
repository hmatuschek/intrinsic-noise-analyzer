#ifndef __INA_APP_DOCTREE_SBMLMODEL_HH__
#define __INA_APP_DOCTREE_SBMLMODEL_HH__

#include <QObject>

#include "models/basemodel.hh"
#include "documenttreeitem.hh"

class ReactionsItem;

/**
 * Represents a @c Ast::Model and associated analysis results for the GUI. Its a containter for
 * everything, that is associated with a certain SBML model.
 *
 * A SBMLModel is also a TreeItem, that allows to browse through model items and results.
 *
 * @ingroup gui
 */
class ModelItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

public:
  /** Constructs a model for the given SBML document. */
  explicit ModelItem(const QString &file_path, QObject *parent = 0);
  /** Constructs a model for the given SBML document. */
  explicit ModelItem(iNA::Ast::Model *_model, QObject *parent = 0);
  /** Destructor. Also frees the held SBML document. */
  ~ModelItem();

  /** Implements @c Wrapper interface. */
  virtual const QString &getLabel() const;

  /** Returns a reference to the SBML model. */
  iNA::Ast::Model &getModel();
  /** Returns a const reference to the SBML model. */
  const iNA::Ast::Model &getModel() const;

  /** Returns a weak reference to the ReactionsItem */
  ReactionsItem *reactionsItem();

  /** Returns true. */
  virtual bool providesView() const;
  /** Create a new model view. */
  virtual QWidget *createView();

  /** Forces an update of the model. */
  void updateItem();

private:
  /** Holds the SBML document associated with the model. */
  iNA::Ast::Model *_model;
  /** Holds the reactions item. */
  ReactionsItem *_reactions;
  /** Holds the display label for the item in the @c DocumentTree. */
  QString itemLabel;
};




#endif // __INA_APP_DOCTREE_SBMLMODEL_HH__
