#ifndef __INA_APP_VIEWS_SPECIESVIEW_HH__
#define __INA_APP_VIEWS_SPECIESVIEW_HH__

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include "../doctree/speciesitem.hh"
#include "../models/expressiondelegate.hh"
#include "../models/compartmentdelegate.hh"
#include "../models/variableruledelegate.hh"
#include "../models/tinytexdelegate.hh"


/**
 * Displays the list of species held by a @c SpeciesItem.
 *
 * @ingroup gui
 */
class SpeciesView : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a new view for the given species item. */
  explicit SpeciesView(SpeciesItem *species, QWidget *parent = 0);

private slots:
  /** The view is connected to the @c SpeciesItem instance, if it gets destroyed, the view
   * is closed. */
  void speciesItemDestoyed();
  /** Adds a species. */
  void onAddSpecies();
  /** Deletes the selected species. */
  void onRemSpecies();
  /** Callback whenever the selection changed. */
  void onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected);

private:
  /** Holds a weak reference to the species item. */
  SpeciesItem *_species;
  /** Holds a simple tinyTex rendering delegate. */
  TinyTexDelegate *tinyTexDelegate;
  /** Holds the ExpressionDelegate, allows to edit expressions. */
  ExpressionDelegate *expressionDelegate;
  /** Holds the CompartmentDelegate, shows a combobox for compartment selection. */
  CompartmentDelegate *compartmentDelegate;
  /** Holds the SpeciesRuleDelegate, shows a simple editor for the species rule. */
  SpeciesRuleDelegate *ruleDelegate;
  /** Holds the QTableView. */
  QTableView *_specTable;
  /** Add species button. */
  QPushButton *_addSpeciesButton;
  /** Delete species button. */
  QPushButton *_remSpeciesButton;
};


#endif // __INA_APP_VIEWS_SPECIESVIEW_HH__
