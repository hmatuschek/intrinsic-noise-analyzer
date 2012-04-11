#ifndef __INA_APP_VIEWS_COMPARTMENTVIEW_HH__
#define __INA_APP_VIEWS_COMPARTMENTVIEW_HH__

#include <QWidget>
#include <QTableView>

#include "../doctree/compartmentsitem.hh"


/**
 * A simple view for the compartments.
 *
 * @ingroup gui
 */
class CompartmentView : public QWidget
{
  Q_OBJECT

protected:
  /**
   * Holds a weak reference to the @c CompartmentsItem instance being displayed.
   */
  CompartmentsItem *compartments;


public:
  /**
   * Constructs a new view for the model compartments.
   */
  explicit CompartmentView(CompartmentsItem *compartments, QWidget *parent = 0);


private slots:
  /**
   * The view is connected to @c destroyed() signal of the @c CompartmentsItem instance, so that
   * the view is closed if the CompartmentsItem is destroyed.
   */
  void compartmentsDestroyed();
};


#endif // __INA_APP_VIEWS_COMPARTMENTVIEW_HH__
