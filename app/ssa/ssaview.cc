#include "ssaview.hh"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "utils/matexport.hh"
#include <fstream>

#include "ssaplot.hh"
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "../plot/configuration.hh"
#include "../plot/plotconfigdialog.hh"
#include "../views/speciesselectiondialog.hh"



/* ********************************************************************************************* *
 * Implementation of SSATaskView
 * ********************************************************************************************* */
SSATaskView::SSATaskView(SSATaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}

QWidget *
SSATaskView::createResultWidget(TaskItem *task_item) {
  return new SSAResultWidget(static_cast<SSATaskWrapper *>(task_item));
}


/* ********************************************************************************************* *
 * Implementation of SSAResultWidget
 * ********************************************************************************************* */
SSAResultWidget::SSAResultWidget(SSATaskWrapper *wrapper, QWidget *parent) :
  QWidget(parent), ssa_task_wrapper(wrapper)
{
  // Set size and layout for panel
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Construct table view for data:
  this->data_view = new QTableView();
  this->data_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->data_view->setModel(ssa_task_wrapper->getTimeSeries());

  // Plot button
  this->plot_button = new QPushButton(tr("Quick plot"));
  QObject::connect(this->plot_button, SIGNAL(clicked()), this, SLOT(showPlot()));

  // Custom plot button
  _genericPlotButton = new QPushButton(tr("Customized plot"));
  QObject::connect(_genericPlotButton, SIGNAL(clicked()), this, SLOT(_genericPlotButtonPressed()));

  // "Save as text..." button.
  this->save_button = new QPushButton(tr("Save data to file"));
  QObject::connect(this->save_button, SIGNAL(clicked()), this, SLOT(saveData()));

  // Layout-box for buttons:
  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(plot_button);
  button_box->addWidget(_genericPlotButton);
  button_box->addWidget(save_button);

  // Panel layout
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_box);
  layout->addWidget(this->data_view);
  this->setLayout(layout);
}


void
SSAResultWidget::showPlot()
{
  // Ask user to select some species:
  SpeciesSelectionDialog dialog(ssa_task_wrapper->getSSATask()->getModel());
  dialog.setWindowTitle(tr("SSA quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Accepted != dialog.exec()) { return; }
  QStringList selected_species = dialog.selectedSpecies();

  // Simply construct and add plot to task:
  Application::getApp()->docTree()->addPlot(
        this->ssa_task_wrapper,
        new PlotItem(
          createSSAPlotConfig(selected_species, this->ssa_task_wrapper->getSSATask())));
  Application::getApp()->docTree()->addPlot(
        this->ssa_task_wrapper,
        new PlotItem(
          createSSACorrelationPlotConfig(selected_species, this->ssa_task_wrapper->getSSATask())));
}

void
SSAResultWidget::_genericPlotButtonPressed()
{
  // Show dialog
  Plot::PlotConfig *config = new Plot::PlotConfig(ssa_task_wrapper->getSSATask()->getTimeSeries());
  Plot::PlotConfigDialog dialog(config);
  if (QDialog::Accepted != dialog.exec()) { return; }
  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(ssa_task_wrapper, new PlotItem(config));
}

void
SSAResultWidget::saveData()
{
  QString selectedFilter;
  QString filename = QFileDialog::getSaveFileName(
        this, tr("Save data as ..."), "",
        tr("Text Files (*.txt *.csv);;Matlab 5 Files (*.mat)"),
        &selectedFilter);
  if ("" == filename) { return; }

  if (tr("Text Files (*.txt *.csv)") == selectedFilter) {
    saveAsCSV(filename);
  } else if (tr("Matlab 5 Files (*.mat)") == selectedFilter) {
    saveAsMAT(filename);
  } else {
    QMessageBox::critical(0, tr("Can not save results."),
                          tr("Can not save results into file %1: Unknown format %2").arg(
                            filename, selectedFilter));
  }
}

void
SSAResultWidget::saveAsCSV(const QString &filename) {
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
    QMessageBox::critical(0, tr("Can not open file"),
                          tr("Can not open file %1 for writing").arg(filename));
    return;
  }

  this->ssa_task_wrapper->getSSATask()->getTimeSeries().saveAsText(file);
  file.close();
}

void
SSAResultWidget::saveAsMAT(const QString &filename) {
  std::fstream file(filename.toLocal8Bit().constData(), std::fstream::out|std::fstream::binary);
  if (! file.is_open()) {
    QMessageBox::critical(0, tr("Can not open file"),
                          tr("Can not open file %1 for writing").arg(filename));
    return;
  }

  iNA::Utils::MatFile mat_file;
  mat_file.add("SSA_result", this->ssa_task_wrapper->getSSATask()->getTimeSeries().matrix());
  mat_file.serialize(file);
  file.close();
}
