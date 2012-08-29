#ifndef __INA_APP_DOCTREE_SBMLMODEL_HH__
#define __INA_APP_DOCTREE_SBMLMODEL_HH__

#include <QObject>

#include "models/basemodel.hh"
#include "documenttreeitem.hh"


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
  explicit ModelItem(Fluc::Ast::Model *model, QObject *parent = 0);
  /** Destructor. Also frees the held SBML document. */
  ~ModelItem();

  /** Implements @c Wrapper interface. */
  virtual const QString &getLabel() const;

  /** Returns a reference to the SBML model. */
  Fluc::Ast::Model &getModel();
  /** Returns a const reference to the SBML model. */
  const Fluc::Ast::Model &getModel() const;

  /** Retruns true. */
  virtual bool providesView() const;
  /** Create a new model view. */
  virtual QWidget *createView();

private:
  /** Holds the SBML document associated with the model. */
  Fluc::Models::BaseModel *base_model;
  /** Holds the display label for the item in the @c DocumentTree. */
  QString itemLabel;
};




#endif // __INA_APP_DOCTREE_SBMLMODEL_HH__
