#ifndef __INA_APP_VIEWS_COMPARTMENTVIEW_HH__
#define __INA_APP_VIEWS_COMPARTMENTVIEW_HH__

#include <QWidget>
#include <QTableView>
#include <QPushButton>

#include "../doctree/compartmentsitem.hh"
#include "../models/expressiondelegate.hh"


/** A simple view for the compartments. */
class CompartmentView : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a new view for the model compartments. */
  explicit CompartmentView(CompartmentsItem *_compartments, QWidget *parent = 0);


private slots:
  /** The view is connected to @c destroyed() signal of the @c CompartmentsItem instance, so that
   * the view is closed if the CompartmentsItem is destroyed. */
  void onCompartmentsDestroyed();
  /** callback for _addCompartment. */
  void onAddCompartment();
  /** callback for _remCompartment. */
  void onRemCompartment();
  /** callback on selection change. */
  void onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected);

private:
  /** Holds a weak reference to the @c CompartmentsItem instance being displayed. */
  CompartmentsItem *_compartments;
  /** Holds the table view of compartments. */
  QTableView *_compTable;
  /** The "add compartment" button. */
  QPushButton *_addCompartment;
  /** The "delete compartment" button. */
  QPushButton *_remCompartment;
};


#endif // __INA_APP_VIEWS_COMPARTMENTVIEW_HH__
