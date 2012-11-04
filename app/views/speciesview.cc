#include "speciesview.hh"
#include "../models/application.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHeaderView>

#include "../models/variableruledata.hh"
#include "../models/expressiondelegate.hh"
#include "../models/compartmentdelegate.hh"
#include "../models/variableruledelegate.hh"


SpeciesView::SpeciesView(SpeciesItem *species ,QWidget *parent) :
  QWidget(parent), _species(species)
{
  // basic Layout settings
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  // Label
  QLabel *label = new QLabel(tr("Species"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Create buttons:
  _addSpeciesButton = new QPushButton("+");
  _addSpeciesButton->setToolTip(tr("Adds a new species."));
  _addSpeciesButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remSpeciesButton = new QPushButton("-");
  _remSpeciesButton->setToolTip(tr("Deletes the selected species"));
  _remSpeciesButton->setEnabled(false);
  _remSpeciesButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Assemble view:
  _specTable = new QTableView();
  _specTable->setModel(_species->species());
  _specTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  _specTable->verticalHeader()->hide();

  // Register delegates for columns:
  _specTable->setItemDelegateForColumn(1, new PixmapDelegate(_specTable));
  _specTable->setItemDelegateForColumn(
        2, new ExpressionDelegate(_species->species()->model(), _specTable));
  _specTable->setItemDelegateForColumn(
        5, new CompartmentDelegate(_species->species()->model(), _specTable));

  // Assemble widget layout
  QHBoxLayout *header = new QHBoxLayout();
  header->addWidget(_addSpeciesButton, 0, Qt::AlignLeft);
  header->addWidget(_remSpeciesButton, 0, Qt::AlignLeft);
  header->addWidget(label, 1, Qt::AlignRight);

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
  // ok, allow removal of species:
  _remSpeciesButton->setEnabled(true);
}
