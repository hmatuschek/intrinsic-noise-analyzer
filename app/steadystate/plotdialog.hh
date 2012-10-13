#ifndef __INA_APP_STEADYSTATE_PLOTDIALOG_HH__
#define __INA_APP_STEADYSTATE_PLOTDIALOG_HH__

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <ast/model.hh>

/** Simple plot dialog to assemble quickly some default plots for the steady state analysis. */
class SteadyStatePlotDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SteadyStatePlotDialog(iNA::Ast::Model *model, QWidget *parent = 0);
  
  /** Retunrs the list of selected species. */
  QList<QString> getSelectedSpecies() const;

  /** Set title text. */
  void setTitle(const QString &text);

private slots:
  /** Calcback on "ok", checks if the configuration is valid. */
  void onAccepted();

private:
  /** A weak reference to the model to create list of species. */
  iNA::Ast::Model *_model;
  /** A label placed over the species list. */
  QLabel *_head_label;
  /** The list widget for species selection. */
  QListWidget *_species_list;
};

#endif // __INA_APP_STEADYSTATE_PLOTDIALOG_HH__
