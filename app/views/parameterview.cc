#include "parameterview.hh"
#include "../application.hh"
#include <QLabel>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "../models/variableruledata.hh"
#include "../models/expressiondelegate.hh"
#include "../models/compartmentdelegate.hh"
#include "../models/variableruledelegate.hh"



/* ********************************************************************************************* *
 * Implementation of ParameterView (pane)
 * ********************************************************************************************* */
ParameterView::ParameterView(ParametersItem *parameters, QWidget *parent) :
  QWidget(parent)
{
  // Basic view layout
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Label
  QLabel *label = new QLabel(tr("Model Parameters"));
  label->setFont(Application::getApp()->getH1Font());
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

  // Create buttons:
  _addParamButton = new QPushButton("+");
  _addParamButton->setToolTip(tr("Adds a new parameter."));
  _addParamButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _remParamButton = new QPushButton("-");
  _remParamButton->setToolTip(tr("Deletes the selected parameter"));
  _remParamButton->setEnabled(false);
  _remParamButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Parameter table
  this->_paramTable = new ParameterTable(parameters->parmeters());
  this->_paramTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  // Register delegates for columns:
  _paramTable->setItemDelegateForColumn(
        1, new ExpressionDelegate(parameters->parmeters()->model(), _paramTable));
  _paramTable->setItemDelegateForColumn(
        2, new ExpressionDelegate(parameters->parmeters()->model(), _paramTable));

  // Layout
  QHBoxLayout *header_layout = new QHBoxLayout();
  header_layout->addWidget(_addParamButton, 0, Qt::AlignLeft);
  header_layout->addWidget(_remParamButton, 0, Qt::AlignLeft);
  header_layout->addWidget(label, 1, Qt::AlignRight);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(header_layout, 0);
  layout->addWidget(this->_paramTable, 0);
  this->setLayout(layout);

  // Connect signals
  QObject::connect(parameters, SIGNAL(destroyed()), this, SLOT(deleteLater()));
  QObject::connect(_addParamButton, SIGNAL(clicked()), this, SLOT(onAddParameter()));
  QObject::connect(_remParamButton, SIGNAL(clicked()), this, SLOT(onRemParameter()));
  QObject::connect(
        _paramTable->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
}


void
ParameterView::onSelectionChanged(const QItemSelection &selected, const QItemSelection &unselected)
{
  QModelIndexList indices = selected.indexes();
  if (1 != indices.size()) { _remParamButton->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remParamButton->setEnabled(false); return; }
  // ok, allow removal of parameters:
  _remParamButton->setEnabled(true);
}


void
ParameterView::onAddParameter()
{
  // Forward call to parameter list model:
  _paramTable->parameters().addParameter();
}


void
ParameterView::onRemParameter() {
  // Check if an identifier of a parameter is selected:
  if (! _paramTable->selectionModel()->hasSelection()) {
    _remParamButton->setEnabled(false);
    return;
  }
  QModelIndexList indices = _paramTable->selectionModel()->selectedIndexes();
  if (1 != indices.size()) { _remParamButton->setEnabled(false); return; }
  QModelIndex index = indices.front();
  if (0 != index.column()) { _remParamButton->setEnabled(false); return; }

  // Tell species list model to remove species:
  _paramTable->parameters().remParameter(index.row());
}



/* ********************************************************************************************* *
 * Implementation of ParameterTable (list view)
 * ********************************************************************************************* */
ParameterTable::ParameterTable(ParameterList *parameters, QWidget *parent)
  : QTableView(parent), _parameters(parameters)
{
  // parameters is a QAbstractTableModel
  this->setModel(parameters);
  this->verticalHeader()->hide();
  //this->horizontalHeader()->setStretchLastSection(true);
  //this->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}


ParameterList & ParameterTable::parameters() { return *_parameters; }



