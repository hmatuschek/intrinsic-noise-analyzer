#include "compartmentview.hh"
#include "../application.hh"
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>



/* ********************************************************************************************* *
 * Implementation of CompartmentView (pane)
 * ********************************************************************************************* */
CompartmentView::CompartmentView(CompartmentsItem *compartments, QWidget *parent) :
  QWidget(parent), compartments(compartments)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  QLabel *label = new QLabel(tr("Model Compartments"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  QTableView *compTable = new QTableView();
  compTable->setModel(this->compartments->compartments());
  compTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  compTable->verticalHeader()->hide();

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0, Qt::AlignRight);
  layout->addWidget(compTable, 0);
  this->setLayout(layout);

  // Receive signal about destruction of CompartmentsWrapper:
  QObject::connect(this->compartments, SIGNAL(destroyed()),
                   this, SLOT(compartmentsDestroyed()));
}



void
CompartmentView::compartmentsDestroyed()
{
  //std::cerr << "CompartmentsWrapper destroyed -> delete CompartmentView." << std::endl;
  this->deleteLater();
}


