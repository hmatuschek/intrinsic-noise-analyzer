#include "paramscantaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include <fstream>
#include "utils/matexport.hh"

#include "paramscanplot.hh"
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "../views/speciesselectiondialog.hh"
#include "../plot/configuration.hh"
#include "../plot/plotconfigdialog.hh"



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

  // Ask user for species to plot.
  SpeciesSelectionDialog dialog(config.getModel());
  dialog.setWindowTitle(tr("Parameter scan quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Accepted != dialog.exec()) { return; }
  QStringList selected_species = dialog.selectedSpecies();

  switch(config.getMethod())
  {
  case ParamScanTask::Config::RE_ANALYSIS:
      // Add RE parameter vs concentration plot:
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanREPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      break;

  case ParamScanTask::Config::LNA_ANALYSIS:
      // Add LNA parameter plot:
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanLNAPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      // Add LNA COV plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanLNACVPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      // Add Fano COV plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanLNAFanoPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      break;

  case ParamScanTask::Config::IOS_ANALYSIS:
      // Add IOS parameter plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanIOSPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      // Add IOS COV plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanIOSCVPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      // Add IOS Fano plot
      Application::getApp()->docTree()->addPlot(
            this->paramscan_task_wrapper,
            new PlotItem(
              createParameterScanIOSFanoPlotConfig(selected_species,this->paramscan_task_wrapper->getParamScanTask())));
      break;

  default:
      break;
  }

}

void
ParamScanResultWidget::customPlotButtonPressed()
{
  // Show dialog
  Plot::PlotConfig *config = new Plot::PlotConfig(
        this->paramscan_task_wrapper->getParamScanTask()->getParameterScan());
  Plot::PlotConfigDialog dialog(config);
  if (QDialog::Accepted != dialog.exec()) { return; }
  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(
        this->paramscan_task_wrapper, new PlotItem(config));
}

void
ParamScanResultWidget::saveButtonPressed()
{
  QString selectedFilter;
  QString csvFilter = tr("Text Files (*.txt *.csv)");
  QString matFilter = tr("Matlab 5 Files (*.mat)");
  QString filters = QString("%1;;%2").arg(csvFilter).arg(matFilter);
  QString filename = QFileDialog::getSaveFileName(
        this, tr("Save as text..."), "", filters, &selectedFilter);
  if ("" == filename) { return; }

  if (csvFilter == selectedFilter) { saveAsCSV(filename); }
  else if (tr("Matlab 5 Files (*.mat)") == selectedFilter) { saveAsMAT(filename); }
  else {
    QMessageBox::critical(0, tr("Can not save results to file"),
                          tr("Can not save results to file %1: Unknown format %2").arg(
                            filename, selectedFilter));
  }
}


void
ParamScanResultWidget::saveAsCSV(const QString &filename)
{
  QFile file(filename);
  // Try to open file
  if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
    QMessageBox::critical(0, tr("Cannot open file"),
                          tr("Cannot open file %1 for writing").arg(filename));
    return;
  }
  // Write...
  paramscan_task_wrapper->getParamScanTask()->getParameterScan().saveAsText(file);
  file.close();
}

void
ParamScanResultWidget::saveAsMAT(const QString &filename) {
  std::fstream file(filename.toLocal8Bit().constData(), std::fstream::out|std::fstream::binary);
  if (! file.is_open()) {
    QMessageBox::critical(0, tr("Can not open file"),
                          tr("Can not open file %1 for writing").arg(filename));
    return;
  }

  iNA::Utils::MatFile mat_file;
  mat_file.add("ParamScan", paramscan_task_wrapper->getParamScanTask()->getParameterScan().matrix());
  mat_file.serialize(file);
  file.close();
}

