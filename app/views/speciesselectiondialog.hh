#ifndef __INA_APP_VIEWS_SPECIESSELECTIONDIALOG_HH__
#define __INA_APP_VIEWS_SPECIESSELECTIONDIALOG_HH__

#include <QDialog>
#include <QListWidget>
#include <QLabel>


// Forward declarations:
namespace iNA { namespace Ast { class Model; } }
class SpeciesSelectionModel;

/** A simple widget that constist of a drop-down selection and a list view. */
class SpeciesSelectionWidget: public QWidget
{
  Q_OBJECT

public:
  /** Constructs the species selection widget. */
  SpeciesSelectionWidget(iNA::Ast::Model *model, QWidget *parent=0);

  /** Returns the selected species. */
  QStringList selectedSpecies();

signals:
  /** Signals that the species selection has changed. */
  void selectionChanged();

private:
  /** The selection model. */
  SpeciesSelectionModel *_model;
};


/** Simple plot to select some species from the model.
 * Mostly used by the quick plots. */
class SpeciesSelectionDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SpeciesSelectionDialog(iNA::Ast::Model *model, QWidget *parent = 0);
  
  /** Retunrs the list of selected species. */
  QList<QString> selectedSpecies() const;

  /** Set title text. */
  void setTitle(const QString &text);

private slots:
  /** Calcback on "ok", checks if the configuration is valid. */
  void _onAccepted();

private:
  /** A label placed over the species list. */
  QLabel *_head_label;
  /** The selection list. */
  SpeciesSelectionWidget *_species_list;
};

#endif
