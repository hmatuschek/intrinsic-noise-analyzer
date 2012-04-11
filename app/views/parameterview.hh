#ifndef __INA_APP_VIEWS_PARAMETERVIEW_HH__
#define __INA_APP_VIEWS_PARAMETERVIEW_HH__

#include <QWidget>
#include <QTableView>

#include "../doctree/parametersitem.hh"



/**
 * Simply extends QTableView to show parameters.
 *
 * Default view for @c ParameterList model.
 *
 * @ingroup gui
 */
class ParameterTable : public QTableView
{
  Q_OBJECT

protected:
  /**
   * A weak reference to the parameter list.
   */
  ParameterList *parameters;


public:
  /**
   * Constructs a view for the parameters.
   */
  ParameterTable(ParameterList *parameters, QWidget *parent=0);
};



/**
 * A simple view widget to show some parameters.
 *
 * Default view for the @c ParametersItem model.
 *
 * @ingroup gui
 */
class ParameterView : public QWidget
{
  Q_OBJECT

public:
  /**
   * Constructs a view to the given paramters.
   */
  explicit ParameterView(ParametersItem *parameters, QWidget *parent = 0);


private slots:
  /**
   * Will be called if the model gets deleted.
   */
  void paramtersDestroyed();


private:
  /**
   * A weak reference to the parameters.
   */
  ParameterTable *paramTable;
};


#endif // __INA_APP_VIEWS_PARAMETERVIEW_HH__
