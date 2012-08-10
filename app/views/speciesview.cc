#include "speciesview.hh"
#include "../application.hh"
#include <QLabel>
#include <QVBoxLayout>
#include <QHeaderView>


SpeciesView::SpeciesView(SpeciesItem *species ,QWidget *parent) :
  QWidget(parent)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  QLabel *label = new QLabel(tr("Model Species"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Create delegate for compartments:
  compartmentDelegate = new CompartmentDelegate(species->species()->model(), this);

  // Assemble view:
  _specTable = new QTableView();
  _specTable->setModel(species->species());
  _specTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  _specTable->verticalHeader()->hide();
  _specTable->setItemDelegateForColumn(5, compartmentDelegate);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0, Qt::AlignRight);
  layout->addWidget(this->_specTable, 0);
  this->setLayout(layout);

  // Connect signals:
  QObject::connect(species, SIGNAL(destroyed()), this, SLOT(speciesItemDestoyed()));
}


void
SpeciesView::speciesItemDestoyed()
{
  //std::cerr << "SpeciesItem destroyed -> close view... " << std::endl;
  this->deleteLater();
}

