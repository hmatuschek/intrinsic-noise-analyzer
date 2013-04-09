#include "lnataskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "lnaplot.hh"
#include "../views/speciesselectiondialog.hh"
#include "../views/genericplotdialog.hh"
#include "../plot/plotconfigdialog.hh"


/* ********************************************************************************************* *
 * Implementation of LNATaskView, derived from TaskView:
 * ********************************************************************************************* */
LNATaskView::LNATaskView(LNATaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
LNATaskView::createResultWidget(TaskItem *task_item) {
  return new LNAResultWidget(static_cast<LNATaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of LNAResultWidget, show the result of a LNA (SSE) analysis.
 * ********************************************************************************************* */
LNAResultWidget::LNAResultWidget(LNATaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), _lna_task_wrapper(task_wrapper)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  _dataTable = new QTableView();
  _dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  _tableWrapper = new TableWrapper(_lna_task_wrapper->getLNATask()->getTimeSeries(), this);
  _dataTable->setModel(this->_tableWrapper);

  _plotButton = new QPushButton(tr("Quick plot"));
  QObject::connect(_plotButton, SIGNAL(clicked()), this, SLOT(_plotButtonPressed()));

  _genericPlotButton = new QPushButton(tr("Customized plot"));
  QObject::connect(_genericPlotButton, SIGNAL(clicked()), this, SLOT(_genericPlotButtonPressed()));

  _saveButton = new QPushButton(tr("Save data to file"));
  QObject::connect(_saveButton, SIGNAL(clicked()), this, SLOT(_saveButtonPressed()));

  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(_plotButton);
  button_box->addWidget(_genericPlotButton);
  button_box->addWidget(_saveButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_box);
  layout->addWidget(_dataTable);
  setLayout(layout);
}


void
LNAResultWidget::_plotButtonPressed() {
  SpeciesSelectionDialog dialog(_lna_task_wrapper->getLNATask()->getConfig().getModel());
  dialog.setWindowTitle(tr("LNA quick plot"));
  dialog.setTitle(tr("Select the species to plot."));

  if (QDialog::Rejected == dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(
        _lna_task_wrapper,
        new PlotItem(
          createLNATimeSeriesPlotConfig(selected_species, _lna_task_wrapper->getLNATask())));

  // Add correlation coefficient plot (if there are more than one species selected).
  if (1 < selected_species.size()) {
    Application::getApp()->docTree()->addPlot(
          _lna_task_wrapper,
          new PlotItem(
            createLNACorrelationPlotConfig(selected_species, _lna_task_wrapper->getLNATask())));
  }
}


void
LNAResultWidget::_genericPlotButtonPressed() {
  // Create empty plot config:
  Plot::PlotConfig *config = new Plot::PlotConfig(
        *(_lna_task_wrapper->getLNATask()->getTimeSeries()));
  // Show dialog
  Plot::PlotConfigDialog dialog(config);
  if (QDialog::Accepted != dialog.exec()) { delete config; return; }
  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(_lna_task_wrapper, new PlotItem(config));
}


void
LNAResultWidget::_saveButtonPressed() {
  // Get filename
  QString filename = QFileDialog::getSaveFileName(
        this, tr("Save as text..."), "", tr("Text Files (*.txt *.csv)"));
  if ("" == filename) { return; }
  QFile file(filename);
  // Try to open file
  if (!file.open(QIODevice::WriteOnly| QIODevice::Text)) {
    QMessageBox box;
    box.setWindowTitle(tr("Cannot open file"));
    box.setText(tr("Cannot open file %1 for writing").arg(filename));
    box.exec();
  }
  // Write...
  _lna_task_wrapper->getLNATask()->getTimeSeries()->saveAsText(file);
  file.close();
}
