#include "retaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "replot.hh"
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "../views/speciesselectiondialog.hh"
#include "../plot/configuration.hh"
#include "../plot/plotconfigdialog.hh"


/* ********************************************************************************************* *
 * Implementation of RETaskView, derived from TaskView:
 * ********************************************************************************************* */
RETaskView::RETaskView(RETaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
RETaskView::createResultWidget(TaskItem *task_item)
{
  return new REResultWidget(static_cast<RETaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of REResultWidget, show the result of a RE analysis.
 * ********************************************************************************************* */
REResultWidget::REResultWidget(RETaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), re_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  this->dataTable = new QTableView();
  this->dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  this->tableWrapper = new TableWrapper(re_task_wrapper->getRETask()->getTimeSeries(), this);
  dataTable->setModel(this->tableWrapper);

  QPushButton *plotButton = new QPushButton(tr("Quick plot"));
  QPushButton *genericPlotButton = new QPushButton(tr("Customized plot"));
  QPushButton *saveButton = new QPushButton(tr("Save data to file"));

  QObject::connect(plotButton, SIGNAL(clicked()), this, SLOT(quickPlotButtonPressed()));
  QObject::connect(genericPlotButton, SIGNAL(clicked()), this, SLOT(genericPlotButtonPressed()));
  QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(saveButtonPressed()));


  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(plotButton);
  button_box->addWidget(genericPlotButton);
  button_box->addWidget(saveButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_box);
  layout->addWidget(this->dataTable);
  this->setLayout(layout);
}


void
REResultWidget::quickPlotButtonPressed() {
  // Create SSE quick plot dialog
  SpeciesSelectionDialog re_dialog(re_task_wrapper->getRETask()->getConfig().getModel());
  re_dialog.setWindowTitle(tr("RE quick plot"));
  re_dialog.setTitle(tr("Select the species to plot."));
  // Exec & get selected species
  if (QDialog::Rejected == re_dialog.exec()) { return; }
  QStringList selected_species = re_dialog.getSelectedSpecies();
  // Create and add timeseries plot:
  Plot::PlotConfig *config = createRETimeSeriesPlotConfig(
        selected_species, re_task_wrapper->getRETask());
  Application::getApp()->docTree()->addPlot(this->re_task_wrapper, new PlotItem(config));
}


void
REResultWidget::genericPlotButtonPressed() {
  // Create empty config:
  Plot::PlotConfig *config = new Plot::PlotConfig(*(re_task_wrapper->getRETask()->getTimeSeries()));
  // Show dialog
  Plot::PlotConfigDialog dialog(config);
  if (QDialog::Accepted != dialog.exec()) { delete config; return; }
  // Add plot:
  Application::getApp()->docTree()->addPlot(re_task_wrapper, new PlotItem(config));
}


void
REResultWidget::saveButtonPressed()
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

  this->re_task_wrapper->getRETask()->getTimeSeries()->saveAsText(file);
  file.close();
}
