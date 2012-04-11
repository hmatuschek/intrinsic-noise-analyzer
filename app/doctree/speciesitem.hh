#ifndef __INA_APP_DOCTREE_SPECIESITEM_HH__
#define __INA_APP_DOCTREE_SPECIESITEM_HH__

#include <QObject>

#include "ast/model.hh"
#include "../models/specieslist.hh"
#include "documenttreeitem.hh"


class Application;


/**
 * Wraps the list of species as a TreeItem (@c Wrapper).
 *
 * @ingroup gui
 */
class SpeciesItem : public QObject, public DocumentTreeItem
{
  Q_OBJECT

protected:
  /**
   * Holds the list of species.
   */
  SpeciesList *_species;

  /**
   * Holds the display name of the tree item, this will be shown in the left application-panel.
   */
  QString itemLabel;


public:
  /**
   * Constructs a new DocumentTree item holding the list of species of the given model.
   */
  SpeciesItem(Fluc::Ast::Model *model, QObject *parent=0);

  /**
   * Returns the list of species.
   */
  SpeciesList *species();

  /**
   * Implements the @c Wrapper interface, returns the display label of the item ("Species").
   */
  virtual const QString &getLabel() const;

  /**
   * Returns true.
   */
  virtual bool providesView() const;

  /**
   * Returns an instance of @c SpeciesView displaying the list of species.
   */
  virtual QWidget *createView();
};



#endif // __INA_APP_DOCTREE_SPECIESITEM_HH__
