#ifndef __INA_APP_VIEWS_SPECIESSELECTIONDIALOG_HH__
#define __INA_APP_VIEWS_SPECIESSELECTIONDIALOG_HH__

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <ast/model.hh>

/** Simple plot to select some species from the model.
 * Mostly used by the quick plots. */
class SpeciesSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SpeciesSelectionDialog(iNA::Ast::Model *model, QWidget *parent = 0);
  
  /** Retunrs the list of selected species. */
  QList<QString> getSelectedSpecies() const;

  /** Set title text. */
  void setTitle(const QString &text);

private slots:
  /** Calcback on "ok", checks if the configuration is valid. */
  void _onAccepted();
  /** Callback to select all species. */
  void _onSelectAllSpecies();
  /** Callback to select no species. */
  void _onSelectNoSpecies();
  /** Callback to invert selection. */
  void _onInvertSelection();

private:
  /** A weak reference to the model to create list of species. */
  iNA::Ast::Model *_model;
  /** A label placed over the species list. */
  QLabel *_head_label;
  /** The list widget custom species selection. */
  QListWidget *_species_list;
};

#endif
