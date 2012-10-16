#include "ssscantaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "ssscanplot.hh"
#include "plotdialog.hh"



/* ********************************************************************************************* *
 * Implementation of TaskView, derived from TaskView:
 * ********************************************************************************************* */
SSScanTaskView::SSScanTaskView(SSScanTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
SSScanTaskView::createResultWidget(TaskItem *task_item)
{
  return new SSScanResultWidget(static_cast<SSScanTaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of ResultWidget, show the result of a Parameter scan.
 * ********************************************************************************************* */
SSScanResultWidget::SSScanResultWidget(SSScanTaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), ssscan_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  this->dataTable = new QTableView();
  this->dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  this->tableWrapper = new TableWrapper(&(ssscan_task_wrapper->getSSScanTask()->getParameterScan()), this);
  dataTable->setModel(this->tableWrapper);

  this->plotButton = new QPushButton(tr("Plot statistics"));
  QObject::connect(this->plotButton, SIGNAL(clicked()), this, SLOT(plotButtonPressed()));

  this->saveButton = new QPushButton(tr("Save data to file"));
  QObject::connect(this->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonPressed()));


  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(this->plotButton);
  button_box->addWidget(this->saveButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_box);
  layout->addWidget(this->dataTable);
  this->setLayout(layout);
}


void
SSScanResultWidget::plotButtonPressed()
{
  // Ask user for species to plot.
  SSScanPlotDialog dialog(ssscan_task_wrapper->getSSScanTask()->getConfig().getModel());
  if (QDialog::Accepted != dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  // Add LNA parameter plot:
  Application::getApp()->docTree()->addPlot(
        this->ssscan_task_wrapper,
        new PlotItem(
          new ParameterScanPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

  // Add IOS parameter plot
  Application::getApp()->docTree()->addPlot(
        this->ssscan_task_wrapper,
        new PlotItem(
          new ParameterScanIOSPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

  // Add IOS COV plot
  Application::getApp()->docTree()->addPlot(
        this->ssscan_task_wrapper,
        new PlotItem(
          new ParameterScanCovPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

  // Add IOS COV plot
  Application::getApp()->docTree()->addPlot(
        this->ssscan_task_wrapper,
        new PlotItem(
          new ParameterScanCovIOSPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));
}


void
SSScanResultWidget::saveButtonPressed()
{
}
