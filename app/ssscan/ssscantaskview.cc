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
#include "../views/speciesselectiondialog.hh"



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

  // Get the task config
  const ParamScanTask::Config &config = ssscan_task_wrapper->getSSScanTask()->getConfig();

  // Ask user for species to plot.
  SpeciesSelectionDialog dialog(config.getModel());
  dialog.setWindowTitle(tr("Parameter scan quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Accepted != dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  switch(config.getMethod())
  {
  case ParamScanTask::Config::RE_ANALYSIS:
      // Add RE parameter vs concentration plot:
      Application::getApp()->docTree()->addPlot(
            this->ssscan_task_wrapper,
            new PlotItem(
              new SimpleParameterScanPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

      break;

  case ParamScanTask::Config::LNA_ANALYSIS:
      // Add LNA parameter plot:
      Application::getApp()->docTree()->addPlot(
            this->ssscan_task_wrapper,
            new PlotItem(
              new ParameterScanPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

      // Add LNA COV plot
      Application::getApp()->docTree()->addPlot(
            this->ssscan_task_wrapper,
            new PlotItem(
              new ParameterScanCovPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

      break;
  case ParamScanTask::Config::IOS_ANALYSIS:
      // Add IOS parameter plot
      Application::getApp()->docTree()->addPlot(
            this->ssscan_task_wrapper,
            new PlotItem(
              new ParameterScanIOSPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));

      // Add IOS COV plot
      Application::getApp()->docTree()->addPlot(
            this->ssscan_task_wrapper,
            new PlotItem(
              new ParameterScanCovIOSPlot(selected_species,this->ssscan_task_wrapper->getSSScanTask())));
      break;
  }

}


void
SSScanResultWidget::saveButtonPressed()
{
}
