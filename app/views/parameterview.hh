#ifndef __INA_APP_VIEWS_PARAMETERVIEW_HH__
#define __INA_APP_VIEWS_PARAMETERVIEW_HH__

#include <QWidget>
#include <QTableView>
#include <QPushButton>

#include "../doctree/parametersitem.hh"



/** Simply extends QTableView to show parameters.
 * Default view custom @c ParameterList model. */
class ParameterTable : public QTableView
{
  Q_OBJECT

protected:
  /** A weak reference to the parameter list. */
  ParameterList *_parameters;

public:
  /** Constructs a view custom the parameters. */
  ParameterTable(ParameterList *_parameters, QWidget *parent=0);

  /** Returns a weak reference to the list of paramters. */
  ParameterList &parameters();
};



/** A simple view widget to show some parameters.
 * Default view custom the @c ParametersItem model. */
class ParameterView : public QWidget
{
  Q_OBJECT

public:
  /** Constructs a view to the given paramters. */
  explicit ParameterView(ParametersItem *parameters, QWidget *parent = 0);

private slots:
  /** Will be called if "add param" is clicked. */
  void onAddParameter();
  /** Will be called if "rem param" is clicked. */
  void onRemParameter();
  /** Will be called if the selection of the parameter list is changed.
   * Is used to enable/disable the "rem param" button. */
  void onSelectionChanged(const QItemSelection &selected,const QItemSelection &unselected);

private:
  /** Holds the paramter table view. */
  ParameterTable *_paramTable;
  /** The "add param" button. */
  QPushButton *_addParamButton;
  /** The "rem param" button. */
  QPushButton *_remParamButton;
};


#endif // __INA_APP_VIEWS_PARAMETERVIEW_HH__
