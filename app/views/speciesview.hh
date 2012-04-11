#ifndef __INA_APP_VIEWS_SPECIESVIEW_HH__
#define __INA_APP_VIEWS_SPECIESVIEW_HH__

#include <QWidget>
#include <QTableView>
#include "../doctree/speciesitem.hh"


/**
 * Displays the list of species held by a @c SpeciesItem.
 *
 * @ingroup gui
 */
class SpeciesView : public QWidget
{
  Q_OBJECT

public:
  /**
   * Constructs a new view for the given species item.
   */
  explicit SpeciesView(SpeciesItem *species, QWidget *parent = 0);


private slots:
  /**
   * The view is connected to the @c SpeciesItem instance, if it gets destroyed, the view
   * is closed.
   */
  void speciesItemDestoyed();


private:
  /**
   * Holds the QTableView.
   */
  QTableView *specTable;
};


#endif // __INA_APP_VIEWS_SPECIESVIEW_HH__
