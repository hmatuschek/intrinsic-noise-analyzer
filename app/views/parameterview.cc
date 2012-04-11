#include "parameterview.hh"
#include "../application.hh"
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>



/* ********************************************************************************************* *
 * Implementation of ParameterView (pane)
 * ********************************************************************************************* */
ParameterView::ParameterView(ParametersItem *parameters, QWidget *parent) :
  QWidget(parent)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  QLabel *label = new QLabel(tr("Model Parameters"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  this->paramTable = new ParameterTable(parameters->parmeters());
  this->paramTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label, 0, Qt::AlignRight);
  layout->addWidget(this->paramTable, 0);
  this->setLayout(layout);

  QObject::connect(parameters, SIGNAL(destroyed()), this, SLOT(paramtersDestroyed()));
}


void
ParameterView::paramtersDestroyed()
{
  //std::cerr << "ParametersItem destroyed -> close ParameterView." << std::endl;
  this->deleteLater();
}




/* ********************************************************************************************* *
 * Implementation of ParameterTable (list)
 * ********************************************************************************************* */
ParameterTable::ParameterTable(ParameterList *parameters, QWidget *parent)
  : QTableView(parent), parameters(parameters)
{
  // parameters is a QAbstractTableModel
  this->setModel(parameters);
  this->verticalHeader()->hide();
  //this->horizontalHeader()->setStretchLastSection(true);
  //this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}




