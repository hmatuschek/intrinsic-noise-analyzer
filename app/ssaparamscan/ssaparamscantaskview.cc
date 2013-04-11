#include "ssaparamscantaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "ssaparamscanplot.hh"
#include "ssaparamscantask.hh"
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "../views/speciesselectiondialog.hh"
#include "../plot/canvas.hh"
#include "../plot/plotconfigdialog.hh"


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
  // get task
  SSAParamScanTaskWrapper *task_wrapper = dynamic_cast<SSAParamScanTaskWrapper *>(task_item);
  return new SSAParamScanResultWidget(task_wrapper);
}

QWidget *
SSAParamScanTaskView::createProgressWidget(TaskItem *task_item)
{
  // get task
  SSAParamScanTaskWrapper *task_wrapper = dynamic_cast<SSAParamScanTaskWrapper *>(task_item);
  if (Task::INITIALIZED == task_wrapper->getTask()->getState())
    return new TaskProgressWidget(task_wrapper);
  else {
    return new SSAParamScanPreviewWidget(task_wrapper);
  }
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

  // Get the task & config
  SSAParamScanTask *task = this->paramscan_task_wrapper->getParamScanTask();
  const SSAParamScanTask::Config &config = task->getConfig();

  // Ask user for species to plot.
  SpeciesSelectionDialog dialog(config.getModel());
  dialog.setWindowTitle(tr("Parameter scan quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Accepted != dialog.exec()) { return; }
  QStringList selected_species = dialog.selectedSpecies();

  // Add SSA variance plot
  Application::getApp()->docTree()->addPlot(
        this->paramscan_task_wrapper,
        new PlotItem(createSSAParameterScanPlotConfig(selected_species, task)));

  // Add SSA CV plot
  Application::getApp()->docTree()->addPlot(
        this->paramscan_task_wrapper,
        new PlotItem(createSSAParameterScanCVPlotConfig(selected_species, task)));

  // Add SSA Fano plot
  Application::getApp()->docTree()->addPlot(
        this->paramscan_task_wrapper,
        new PlotItem(createSSAParameterScanFanoPlotConfig(selected_species, task)));


}

void
SSAParamScanResultWidget::customPlotButtonPressed()
{
  // Show dialog
  Plot::PlotConfig *config = new Plot::PlotConfig(
        this->paramscan_task_wrapper->getParamScanTask()->getParameterScan());
  Plot::PlotConfigDialog dialog(config);
  if (QDialog::Accepted != dialog.exec()) { delete config; return; }

  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(this->paramscan_task_wrapper, new PlotItem(config));
}

void
SSAParamScanResultWidget::saveButtonPressed()
{
  QString filename = QFileDialog::getSaveFileName(
        this, tr("Save as text..."), "", tr("Text Files (*.txt *.csv)"));
  if ("" == filename) { return; }

  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
    QMessageBox box;
    box.setWindowTitle(tr("Can not open file"));
    box.setText(tr("Can not open file %1 for writing").arg(filename));
    box.exec();
  }

  this->paramscan_task_wrapper->getParamScanTask()->getParameterScan().saveAsText(file);
  file.close();
}




/* ********************************************************************************************* *
 * Implementation of PreviewWidget
 * ********************************************************************************************* */
SSAParamScanPreviewWidget::SSAParamScanPreviewWidget(SSAParamScanTaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), _task_item(task_wrapper), _updateTimer(), _plottype(CONCENTRATION_PLOT)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(task_wrapper->getTask());

  // Preview
  _plot_canvas = new Plot::Canvas();
  _plot_canvas->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  // Button to select plot
  QPushButton *plot_type_button = new QPushButton(tr("Select plot type"));
  QMenu *plot_menu = new QMenu();
  plot_menu->addAction(tr("Concentrations"), this, SLOT(onConcentrationPlotSelected()));
  plot_menu->addAction(tr("Coefficient of variation"), this, SLOT(onCOVPlotSelected()));
  plot_menu->addAction(tr("Fano factor"), this, SLOT(onFanoPlotSelected()));
  plot_type_button->setMenu(plot_menu);
  // Label to show number of iteration and simulation time
  _iteration_label = new QLabel("");
  _iteration_label->setTextFormat(Qt::LogText);
  // List of species
  _species_list = new SpeciesSelectionWidget(task->getConfig().getModel());
  _species_list->selectAllSpecies();
  _species_list->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  // Button to stop simulation
  QPushButton *done_button = new QPushButton(tr("Done"));
  QPushButton *reset_button = new QPushButton(tr("Reset Statistics"));

  // Assemble layout
  QHBoxLayout *layout = new QHBoxLayout();
  QHBoxLayout *plot_header = new QHBoxLayout();
  plot_header->addWidget(plot_type_button);
  plot_header->addWidget(_iteration_label);
  QVBoxLayout *plot_layout = new QVBoxLayout();
  plot_layout->addWidget(_plot_canvas, 1);
  plot_layout->addLayout(plot_header);
  QVBoxLayout *species_list_layout = new QVBoxLayout();
  species_list_layout->addWidget(_species_list, 1);
  species_list_layout->addWidget(reset_button);
  species_list_layout->addWidget(done_button);
  layout->addLayout(plot_layout);
  layout->addLayout(species_list_layout);
  setLayout(layout);

  // Connect some signals
  QObject::connect(_species_list, SIGNAL(selectionChanged()), this, SLOT(onScheduleUpdatePlot()));
  QObject::connect(done_button, SIGNAL(clicked()), this, SLOT(onDone()));
  QObject::connect(reset_button, SIGNAL(clicked()), this, SLOT(onReset()));
  QObject::connect(task, SIGNAL(stepPerformed()), this, SLOT(onScheduleUpdatePlot()));
  QObject::connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updatePlot()));

  // update plot...
  updatePlot();
}


void
SSAParamScanPreviewWidget::onScheduleUpdatePlot()
{
  // If timer still runs -> done
  if (_updateTimer.isActive()) { return; }
  // Schedule update of plot for later...
  _updateTimer.start(500);
}


void
SSAParamScanPreviewWidget::updatePlot()
{
  // Get selected species list
  QStringList selected_species = _species_list->selectedSpecies();
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());

  // Update label:
  _iteration_label->setText(
        tr("Sample size (simulation time): %1 (%2)").arg(
          task->currentIteration()).arg(task->currentTime()));

  // Remove and destroy "old" plot
  Plot::Figure *old_plot = _plot_canvas->getPlot();
  if (0 != old_plot) { old_plot->deleteLater(); }

  // Update plot
  switch(_plottype)
  {
  case CONCENTRATION_PLOT:
    _plot_canvas->setPlot(createSSAParameterScanPlot(selected_species, task)); break;
  case FANO_PLOT:
    _plot_canvas->setPlot(createSSAParameterScanFanoPlot(selected_species, task)); break;
  case COEFVAR_PLOT:
    _plot_canvas->setPlot(createSSAParameterScanCVPlot(selected_species, task)); break;
  }

}


void
SSAParamScanPreviewWidget::onDone()
{
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());
  task->stopIteration();
}

void
SSAParamScanPreviewWidget::onReset()
{
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());
  task->resetStatistics();
}

void
SSAParamScanPreviewWidget::onConcentrationPlotSelected() {
  _plottype = CONCENTRATION_PLOT;
}

void
SSAParamScanPreviewWidget::onCOVPlotSelected() {
  _plottype = COEFVAR_PLOT;
}

void
SSAParamScanPreviewWidget::onFanoPlotSelected() {
  _plottype = FANO_PLOT;
}
