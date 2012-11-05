#include "paramscantaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../models/application.hh"
#include "../doctree/plotitem.hh"
#include "../views/genericplotdialog.hh"
#include "paramscanplot.hh"
#include "../views/speciesselectiondialog.hh"



/* ********************************************************************************************* *
 * Implementation of TaskView, derived from TaskView:
 * ********************************************************************************************* */
ParamScanTaskView::ParamScanTaskView(ParamScanTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
ParamScanTaskView::createResultWidget(TaskItem *task_item)
{
  return new ParamScanResultWidget(static_cast<ParamScanTaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of ResultWidget, show the result of a Parameter scan.
 * ********************************************************************************************* */
ParamScanResultWidget::ParamScanResultWidget(ParamScanTaskWrapper *task_wrapper, QWidget *parent):
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
ParamScanResultWidget::plotButtonPressed()
{

  // Get the task config
  const ParamScanTask::Config &config = paramscan_task_wrapper->getParamScanTask()->getConfig();

  // Ask user custom species to plot.
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
            this->paramscan_task_wrapper,
            new PlotItem(
              new SimpleParameterScanPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));

      break;

  case ParamScanTask::Config::LNA_ANALYSIS:
      // Add LNA parameter plot:
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              new ParameterScanPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));

      // Add LNA COV plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              new ParameterScanCovPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));

      break;
  case ParamScanTask::Config::IOS_ANALYSIS:
      // Add IOS parameter plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              new ParameterScanIOSPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));

      // Add IOS COV plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              new ParameterScanCovIOSPlot(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      break;
  default:
      break;
  }

}

void
ParamScanResultWidget::customPlotButtonPressed()
{
  // Show dialog
  GenericPlotDialog dialog(&this->paramscan_task_wrapper->getParamScanTask()->getParameterScan());
  if (QDialog::Rejected == dialog.exec()) { return; }

  // Create plot figure with labels.
  Plot::Figure *figure = new Plot::Figure(dialog.figureTitle());
  figure->getAxis()->setXLabel(dialog.xLabel());
  figure->getAxis()->setYLabel(dialog.yLabel());

  // Iterate over all graphs of the configured plot:
  custom (size_t i=0; i<dialog.numGraphs(); i++) {
    figure->getAxis()->addGraph(dialog.graph(i).create(figure->getStyle(i)));
  }

  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(this->paramscan_task_wrapper, new PlotItem(figure));
}

void
ParamScanResultWidget::saveButtonPressed()
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
      box.setText(tr("Can not open file %1 custom writing").arg(filename));
      box.exec();
    }

    this->paramscan_task_wrapper->getParamScanTask()->getParameterScan().saveAsText(file);
    file.close();

}
