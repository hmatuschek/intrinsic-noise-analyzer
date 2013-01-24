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
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(task_wrapper->getTask());
  // Show result widget if task is finally done:
  if (task->isFinal())
    return new SSAParamScanResultWidget(task_wrapper);
  return new SSAParamScanPreviewWidget(task_wrapper);
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
  QWidget(parent), _task_item(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Preview
  _plot_canvas = new Plot::Canvas();
  _species_list = new QListWidget();
  _species_list_label = new QLabel();

  QPushButton *continue_button = new QPushButton(tr("continue simulation"));
  QPushButton *done_button = new QPushButton(tr("done"));

  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(task_wrapper->getTask());
  // populate species list:
  for (size_t i=0; i<task->getConfig().getModel()->numSpecies(); i++) {
    iNA::Ast::Species *species = task->getConfig().getModel()->getSpecies(i);
    QString id   = species->getIdentifier().c_str();
    QString name = id;
    if (species->hasName()) {
      name = species->getName().c_str();
    }

    QListWidgetItem *item = new QListWidgetItem(name, _species_list);
    item->setData(Qt::UserRole, id);
    item->setCheckState(Qt::Checked);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
    _species_list->addItem(item);
  }

  QHBoxLayout *plot_layout = new QHBoxLayout();
  plot_layout->addWidget(_plot_canvas, 1);

  QVBoxLayout *species_list_layout = new QVBoxLayout();
  species_list_layout->addWidget(_species_list_label, 0);
  species_list_layout->addWidget(_species_list, 1);
  plot_layout->addLayout(species_list_layout);

  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(done_button);
  button_box->addWidget(continue_button);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(plot_layout);
  layout->addLayout(button_box);
  setLayout(layout);

  QObject::connect(_species_list, SIGNAL(itemChanged(QListWidgetItem*)),
                   this, SLOT(onItemChanged(QListWidgetItem*)));
  QObject::connect(continue_button, SIGNAL(clicked()), this, SLOT(onContinue()));
  QObject::connect(done_button, SIGNAL(clicked()), this, SLOT(onDone()));

  // update plot...
  onUpdatePlot();
}


void
SSAParamScanPreviewWidget::onItemChanged(QListWidgetItem *item)
{
  onUpdatePlot();
}


void
SSAParamScanPreviewWidget::onUpdatePlot()
{
  QStringList selected_species;
  // Get species list
  for (int i=0; i<_species_list->count(); i++) {
    QListWidgetItem *item = _species_list->item(i);
    if (Qt::Checked == item->checkState()) {
      selected_species.append(item->data(Qt::UserRole).toString());
    }
  }

  Plot::Figure *old_plot = _plot_canvas->getPlot();
  if (0 != old_plot) { old_plot->deleteLater(); }

  // Update plot
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());
  _plot_canvas->setPlot(new SSAParameterScanPlot(selected_species, task));
}


void
SSAParamScanPreviewWidget::onContinue()
{
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());
  if (task->isFinal()) { return; }
  task->start();
  task->setState(Task::DONE);
}


void
SSAParamScanPreviewWidget::onDone()
{
  SSAParamScanTask *task = dynamic_cast<SSAParamScanTask *>(_task_item->getTask());
  if (task->isFinal()) { return; }
  task->setFinal();
  task->setState(Task::DONE);
}


