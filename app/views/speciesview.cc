#include "speciesview.hh"
#include "../application.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHeaderView>


SpeciesView::SpeciesView(SpeciesItem *species ,QWidget *parent) :
  QWidget(parent), _species(species)
{
  // basic Layout settings
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Label
  QLabel *label = new QLabel(tr("Model Species"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Create buttons:
  _addSpeciesButton = new QPushButton("+");
  _addSpeciesButton->setToolTip(tr("Adds a new species."));
  _remSpeciesButton = new QPushButton("-");
  _remSpeciesButton->setToolTip(tr("Deletes the selected species"));
  _remSpeciesButton->setEnabled(false);

  // Create delegate for compartments:
  compartmentDelegate = new CompartmentDelegate(species->species()->model(), this);
  // Create delegate for rules:
  ruleDelegate = new SpeciesRuleDelegate(species->species()->model(), this);

  // Assemble view:
  _specTable = new QTableView();
  _specTable->setModel(_species->species());
  _specTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  _specTable->verticalHeader()->hide();

  // Register delegates for columns:
  _specTable->setItemDelegateForColumn(5, compartmentDelegate);
  _specTable->setItemDelegateForColumn(6, ruleDelegate);

  // Assemble widget layout
  QHBoxLayout *header = new QHBoxLayout();
  header->addWidget(_addSpeciesButton, 0, Qt::AlignLeft);
  header->addWidget(_remSpeciesButton, 0, Qt::AlignLeft);
  header->addWidget(label, 0, Qt::AlignRight);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(header, 0);
  layout->addWidget(_specTable, 0);
  this->setLayout(layout);

  // Connect signals:
  QObject::connect(_addSpeciesButton, SIGNAL(clicked()), this, SLOT(onAddSpecies()));
  QObject::connect(_remSpeciesButton, SIGNAL(clicked()), this, SLOT(onRemSpecies()));
  QObject::connect(
        _specTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
  QObject::connect(_species, SIGNAL(destroyed()), this, SLOT(speciesItemDestoyed()));
}


void
SpeciesView::speciesItemDestoyed()
{
  //std::cerr << "SpeciesItem destroyed -> close view... " << std::endl;
  this->deleteLater();
}


void
SpeciesView::onAddSpecies() {
  // Forward call to species list model:
  _species->species()->addSpecies();
}

void
SpeciesView::onRemSpecies()
{
  // Check if an identifier of a species is selected:
  if (! _specTable->selectionModel()->hasSelection()) {
    _remSpeciesButton->setEnabled(false);
    return;
  }
  QModelIndexList indices = _specTable->selectionModel()->selectedIndexes();
  if (1 != indices.size()) { _remSpeciesButton->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remSpeciesButton->setEnabled(false); return; }

  // Tell species list model to remove species:
  _species->species()->remSpecies(index.row());
}

void
SpeciesView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) { _remSpeciesButton->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remSpeciesButton->setEnabled(false); return; }
}
