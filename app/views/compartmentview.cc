#include "compartmentview.hh"
#include "../application.hh"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>



/* ********************************************************************************************* *
 * Implementation of CompartmentView (pane)
 * ********************************************************************************************* */
CompartmentView::CompartmentView(CompartmentsItem *compartments, QWidget *parent) :
  QWidget(parent), _compartments(compartments)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  // Create buttons:
  _addCompartment = new QPushButton("+");
  _addCompartment->setToolTip(tr("Adds a new compartment to the list."));
  _addCompartment->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remCompartment = new QPushButton("-");
  _remCompartment->setToolTip(tr("Deletes the selected compartment."));
  _remCompartment->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remCompartment->setEnabled(false);

  // The label
  QLabel *label = new QLabel(tr("Model Compartments"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Assemble header
  QHBoxLayout *header = new QHBoxLayout();
  header->addWidget(_addCompartment, 0, Qt::AlignLeft);
  header->addWidget(_remCompartment, 0, Qt::AlignLeft);
  header->addWidget(label, 1, Qt::AlignRight);

  // Create table view:
  _compTable = new QTableView();
  _compTable->setModel(this->_compartments->compartments());
  _compTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  _compTable->verticalHeader()->hide();

  // Create delegate for expressions:
  _compTable->setItemDelegateForColumn(
        1, new ExpressionDelegate(_compartments->compartments()->model(), this));
  _compTable->setItemDelegateForColumn(
        2, new ExpressionDelegate(_compartments->compartments()->model(), this));

  // Assemble layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(header, 0);
  layout->addWidget(_compTable, 0);
  this->setLayout(layout);

  // Connect to some signals:
  QObject::connect(_addCompartment, SIGNAL(clicked()), this, SLOT(onAddCompartment()));
  QObject::connect(_remCompartment, SIGNAL(clicked()), this, SLOT(onRemCompartment()));
  QObject::connect(
        _compTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
  QObject::connect(this->_compartments, SIGNAL(destroyed()),
                   this, SLOT(onCompartmentsDestroyed()));
}



void
CompartmentView::onCompartmentsDestroyed()
{
  //std::cerr << "CompartmentsWrapper destroyed -> delete CompartmentView." << std::endl;
  this->deleteLater();
}


void
CompartmentView::onAddCompartment()
{
  // Simply forward call to compartment list model:
  _compartments->compartments()->addCompartment();
}

void
CompartmentView::onRemCompartment()
{
  // Check if an identifier of a species is selected:
  if (! _compTable->selectionModel()->hasSelection()) {
    _remCompartment->setEnabled(false);
    return;
  }
  QModelIndexList indices = _compTable->selectionModel()->selectedIndexes();
  if (1 != indices.size()) { _remCompartment->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remCompartment->setEnabled(false); return; }

  // Tell species list model to remove species:
  _compartments->compartments()->remCompartment(index.row());
}


void
CompartmentView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) { _remCompartment->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remCompartment->setEnabled(false); return; }
  // ok, allow removal of species:
  _remCompartment->setEnabled(true);
}
