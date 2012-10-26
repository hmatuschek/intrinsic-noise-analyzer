#include "speciesselectiondialog.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMenu>

using namespace iNA;


SpeciesSelectionDialog::SpeciesSelectionDialog(Ast::Model *model, QWidget *parent)
  : QDialog(parent), _model(model)
{
  // Allocate label.
  _head_label = new QLabel();
  _head_label->setVisible(false);

  _species_list = new QListWidget();
  for(size_t i=0; i<_model->numSpecies(); i++)
  {
    // Determine name of species:
    QString name = _model->getSpecies(i)->getIdentifier().c_str();
    QString id   = _model->getSpecies(i)->getIdentifier().c_str();
    if (_model->getSpecies(i)->hasName()) {
      name = _model->getSpecies(i)->getName().c_str();
    }

    // Create list item for species
    QListWidgetItem *item = new QListWidgetItem(name, _species_list);
    item->setData(Qt::UserRole, id);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    _species_list->addItem(item);
  }

  QPushButton *selection_button = new QPushButton("Select");
  QMenu *selection_menu = new QMenu();
  selection_menu->addAction(tr("Select all species"), this, SLOT(_onSelectAllSpecies()));
  selection_menu->addAction(tr("Unselect all species"), this, SLOT(_onSelectNoSpecies()));
  selection_menu->addAction(tr("Invert selection"), this, SLOT(_onInvertSelection()));
  selection_button->setMenu(selection_menu);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QVBoxLayout *list_layout = new QVBoxLayout();
  list_layout->setMargin(0); list_layout->setSpacing(2);
  list_layout->addWidget(selection_button);
  list_layout->addWidget(_species_list);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_head_label);
  layout->addLayout(list_layout);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(_onAccepted()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


QList<QString>
SpeciesSelectionDialog::getSelectedSpecies() const {
  // Assemble list of enabled species:
  QList<QString> selected_species;
  for(size_t i=0; i<_model->numSpecies(); i++) {
    if (Qt::Checked == _species_list->item(i)->checkState()) {
      selected_species.append(
            _species_list->item(i)->data(Qt::UserRole).toString());
    }
  }
  return selected_species;
}


void
SpeciesSelectionDialog::setTitle(const QString &text) {
  _head_label->setText(text);
  _head_label->setVisible(true);
}

void
SpeciesSelectionDialog::_onAccepted() {
  QList<QString> selected_species = getSelectedSpecies();
  if (0 == selected_species.size()) { return; }
  accept();
}

void
SpeciesSelectionDialog::_onSelectAllSpecies()
{
  for (int i=0; i<_species_list->count(); i++) {
    _species_list->item(i)->setCheckState(Qt::Checked);
  }
}

void
SpeciesSelectionDialog::_onSelectNoSpecies()
{
  for (int i=0; i<_species_list->count(); i++) {
    _species_list->item(i)->setCheckState(Qt::Unchecked);
  }
}

void
SpeciesSelectionDialog::_onInvertSelection()
{
  for (int i=0; i<_species_list->count(); i++) {
    if (Qt::Checked == _species_list->item(i)->checkState()) {
      _species_list->item(i)->setCheckState(Qt::Unchecked);
    } else {
      _species_list->item(i)->setCheckState(Qt::Checked);
    }
  }
}
