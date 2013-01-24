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




/* ********************************************************************************************* *
 * Implementation of PreviewWidget
 * ********************************************************************************************* */
SSAParamScanPreviewWidget::SSAParamScanPreviewWidget(SSAParamScanTaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), _task_item(task_wrapper), _updateTimer()
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Preview
  _plot_canvas = new Plot::Canvas();
  _plot_canvas->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  // List of species
  _species_list = new QListWidget();
  _species_list->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  // Button to select species
  QPushButton *selection_button = new QPushButton("Select");
  QMenu *selection_menu = new QMenu();
  selection_menu->addAction(tr("Select all species"), this, SLOT(onSelectAllSpecies()));
  selection_menu->addAction(tr("Unselect all species"), this, SLOT(onSelectNoSpecies()));
  selection_menu->addAction(tr("Invert selection"), this, SLOT(onInvertSelection()));
  selection_button->setMenu(selection_menu);
  // Button to stop simulation
  QPushButton *done_button = new QPushButton(tr("done"));
  // Label to show number of iteration and simulation time
  _iteration_label = new QLabel("");
  _iteration_label->setTextFormat(Qt::LogText);

  // Populate species list
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(task_wrapper->getTask());
  for (size_t i=0; i<task->getConfig().getModel()->numSpecies(); i++) {
    // Get name & id
    iNA::Ast::Species *species = task->getConfig().getModel()->getSpecies(i);
    QString id   = species->getIdentifier().c_str();
    QString name = id;
    if (species->hasName()) { name = species->getName().c_str(); }
    // Assemble item
    QListWidgetItem *item = new QListWidgetItem(name, _species_list);
    item->setData(Qt::UserRole, id);
    item->setCheckState(Qt::Checked);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    _species_list->addItem(item);
  }

  // Assemble layout
  QHBoxLayout *plot_layout = new QHBoxLayout();
  plot_layout->addWidget(_plot_canvas, 1);
  QVBoxLayout *species_list_layout = new QVBoxLayout();
  species_list_layout->addWidget(selection_button, 0);
  species_list_layout->addWidget(_species_list, 1);
  species_list_layout->addWidget(done_button);
  plot_layout->addLayout(species_list_layout);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(plot_layout);
  layout->addWidget(_iteration_label);
  setLayout(layout);

  // Connect some signals
  QObject::connect(_species_list, SIGNAL(itemChanged(QListWidgetItem*)),
                   this, SLOT(onItemChanged(QListWidgetItem*)));
  QObject::connect(done_button, SIGNAL(clicked()), this, SLOT(onDone()));
  QObject::connect(task, SIGNAL(stepPerformed()), this, SLOT(onScheduleUpdatePlot()));
  QObject::connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updatePlot()));

  // update plot...
  updatePlot();
}


void
SSAParamScanPreviewWidget::onItemChanged(QListWidgetItem *item)
{
  onScheduleUpdatePlot();
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
  QStringList selected_species;
  for (int i=0; i<_species_list->count(); i++) {
    QListWidgetItem *item = _species_list->item(i);
    if (Qt::Checked == item->checkState()) {
      selected_species.append(item->data(Qt::UserRole).toString());
    }
  }

  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());

  // Update label:
  _iteration_label->setText(
        tr("Current iteration (simulation time): %1 (%2)").arg(
          task->currentIteration()).arg(task->currentTime()));

  // Remove and destroy "old" plot
  Plot::Figure *old_plot = _plot_canvas->getPlot();
  if (0 != old_plot) { old_plot->deleteLater(); }

  // Update plot
  _plot_canvas->setPlot(new SSAParameterScanPlot(selected_species, task));
}


void
SSAParamScanPreviewWidget::onDone()
{
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());
  task->stopIteration();
}


void
SSAParamScanPreviewWidget::onSelectAllSpecies()
{
  for (int i=0; i<_species_list->count(); i++) {
    QListWidgetItem *item = _species_list->item(i);
    item->setCheckState(Qt::Checked);
  }
}

void
SSAParamScanPreviewWidget::onSelectNoSpecies()
{
  for (int i=0; i<_species_list->count(); i++) {
    QListWidgetItem *item = _species_list->item(i);
    item->setCheckState(Qt::Unchecked);
  }
}

void
SSAParamScanPreviewWidget::onInvertSelection()
{
  for (int i=0; i<_species_list->count(); i++) {
    QListWidgetItem *item =_species_list->item(i);
    if (Qt::Checked == item->checkState())
      item->setCheckState(Qt::Unchecked);
    else
      item->setCheckState(Qt::Checked);
  }
}

