#include "ssaparamscantaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../models/application.hh"
#include "../doctree/plotitem.hh"
#include "../views/genericplotdialog.hh"
#include "ssaparamscanplot.hh"
#include "../views/speciesselectiondialog.hh"



/* ********************************************************************************************* *
 * Implementation of TaskView, derived from TaskView:
 * ********************************************************************************************* */
SSAParamScanTaskView::SSAParamScanTaskView(SSAParamScanTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
SSAParamScanTaskView::createResultWidget(TaskItem *task_item)
{
  return new SSAParamScanResultWidget(static_cast<SSAParamScanTaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of ResultWidget, show the result of a Parameter scan.
 * ********************************************************************************************* */
SSAParamScanResultWidget::SSAParamScanResultWidget(SSAParamScanTaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), paramscan_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  this->dataTable = new QTableView();
  this->dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  this->tableWrapper = new TableWrapper(&(paramscan_task_wrapper->getParamScanTask()->getParameterScan()), this);
  dataTable->setModel(this->tableWrapper);

  this->plotButton = new QPushButton(tr("Quick plot"));
  QObject::connect(this->plotButton, SIGNAL(clicked()), this, SLOT(plotButtonPressed()));

  this->customPlotButton = new QPushButton(tr("Customized plot"));
  QObject::connect(customPlotButton, SIGNAL(clicked()), this, SLOT(customPlotButtonPressed()));

  this->saveButton = new QPushButton(tr("Save data to file"));
  QObject::connect(this->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonPressed()));


  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(this->plotButton);
  button_box->addWidget(this->customPlotButton);
  button_box->addWidget(this->saveButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_box);
  layout->addWidget(this->dataTable);
  this->setLayout(layout);
}


void
SSAParamScanResultWidget::plotButtonPressed()
{

  // Get the task config
  const SSAParamScanTask::Config &config = paramscan_task_wrapper->getParamScanTask()->getConfig();

  // Ask user for species to plot.
  SpeciesSelectionDialog dialog(config.getModel());
  dialog.setWindowTitle(tr("Parameter scan quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Accepted != dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  // Add SSA variance plot
  Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              new SSAParameterScanPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));

  // Add SSA COV plot
  Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              new SSAParameterScanCovPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));

}

void
SSAParamScanResultWidget::customPlotButtonPressed()
{
  // Show dialog
  GenericPlotDialog dialog(&this->paramscan_task_wrapper->getParamScanTask()->getParameterScan());
  if (QDialog::Rejected == dialog.exec()) { return; }

  // Create plot figure with labels.
  Plot::Figure *figure = new Plot::Figure(dialog.figureTitle());
  figure->getAxis()->setXLabel(dialog.xLabel());
  figure->getAxis()->setYLabel(dialog.yLabel());

  // Iterate over all graphs of the configured plot:
  for (size_t i=0; i<dialog.numGraphs(); i++) {
    Plot::Graph *graph = dialog.graph(i).create(figure->getStyle(i));
    figure->getAxis()->addGraph(graph);
    figure->addToLegend(dialog.graph(i).label(), graph);
  }

  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(this->paramscan_task_wrapper, new PlotItem(figure));
}

void
SSAParamScanResultWidget::saveButtonPressed()
{

    QString filename = QFileDialog::getSaveFileName(
          this, tr("Save as text..."), "", tr("Text Files (*.txt *.csv)"));

    if ("" == filename)
    {
      return;
    }

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly| QIODevice::Text))
    {
      QMessageBox box;
      box.setWindowTitle(tr("Can not open file"));
      box.setText(tr("Can not open file %1 for writing").arg(filename));
      box.exec();
    }

    this->paramscan_task_wrapper->getParamScanTask()->getParameterScan().saveAsText(file);
    file.close();

}
