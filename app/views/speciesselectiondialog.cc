#include "speciesselectiondialog.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMenu>
#include <QTableView>
#include <QHeaderView>

#include <ast/model.hh>
#include "../models/speciesselectionmodel.hh"

using namespace iNA;


/* ******************************************************************************************* *
 * Implements the species selection widget
 * ******************************************************************************************* */
SpeciesSelectionWidget::SpeciesSelectionWidget(Ast::Model *model, QWidget *parent)
  : QWidget(parent)
{
  // Create selection model
  _model = new SpeciesSelectionModel(model);

  // Widgets
  QPushButton *select_button = new QPushButton(tr("Select"));
  QMenu *menu = new QMenu();
  menu->addAction(tr("Select all species"), _model, SLOT(selectAllSpecies()));
  menu->addAction(tr("Select no species"), _model, SLOT(selectNoSpecies()));
  menu->addAction(tr("Invert selection"), _model, SLOT(invertSelection()));
  select_button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  select_button->setMenu(menu);

  QTableView *species_list = new QTableView();
  species_list->horizontalHeader()->setStretchLastSection(true);
  species_list->horizontalHeader()->setVisible(false);
  species_list->verticalHeader()->setVisible(false);
  species_list->setModel(_model);
  species_list->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  // Layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->setMargin(0); layout->setSpacing(2);
  layout->addWidget(select_button);
  layout->addWidget(species_list);
  setLayout(layout);

  // Forward selectionChanged signal:
  QObject::connect(_model, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
}

QStringList
SpeciesSelectionWidget::selectedSpecies() {
  return _model->selectedSpecies();
}

void
SpeciesSelectionWidget::selectAllSpecies() {
  _model->selectAllSpecies();
}

void
SpeciesSelectionWidget::selectNoSpecies() {
  _model->selectNoSpecies();
}

void
SpeciesSelectionWidget::invertSelection() {
  _model->invertSelection();
}


/* ******************************************************************************************* *
 * Implements the species selection dialog.
 * ******************************************************************************************* */
SpeciesSelectionDialog::SpeciesSelectionDialog(Ast::Model *model, QWidget *parent)
  : QDialog(parent)
{
  // Allocate label.
  _head_label = new QLabel();
  _head_label->setVisible(false);

  _species_list = new SpeciesSelectionWidget(model);
  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_head_label);
  layout->addWidget(_species_list);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(_onAccepted()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


QList<QString>
SpeciesSelectionDialog::selectedSpecies() const {
  return _species_list->selectedSpecies();
}


void
SpeciesSelectionDialog::setTitle(const QString &text) {
  _head_label->setText(text);
  _head_label->setVisible(true);
}

void
SpeciesSelectionDialog::_onAccepted() {
  QList<QString> selected_species = selectedSpecies();
  if (0 == selected_species.size()) { return; }
  accept();
}

