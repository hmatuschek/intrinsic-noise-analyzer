#include "iostaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../models/application.hh"
#include "../doctree/plotitem.hh"
#include "iosplot.hh"
#include "../views/speciesselectiondialog.hh"
#include "../views/genericplotdialog.hh"


/* ********************************************************************************************* *
 * Implementation of RETaskView, derived from TaskView:
 * ********************************************************************************************* */
IOSTaskView::IOSTaskView(IOSTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
IOSTaskView::createResultWidget(TaskItem *task_item)
{
  return new IOSResultWidget(static_cast<IOSTaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of REResultWidget, show the result of a RE analysis.
 * ********************************************************************************************* */
IOSResultWidget::IOSResultWidget(IOSTaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), _ios_task_wrapper(task_wrapper)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  setBackgroundRole(QPalette::Window);

  _dataTable = new QTableView();
  _dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  _tableWrapper = new TableWrapper(_ios_task_wrapper->getIOSTask()->getTimeSeries(), this);
  _dataTable->setModel(_tableWrapper);

  _plotButton = new QPushButton(tr("Quick plot"));
  QObject::connect(_plotButton, SIGNAL(clicked()), this, SLOT(_onPlotButtonPressed()));

  _genericPlotButton = new QPushButton(tr("Customized plot"));
  QObject::connect(_genericPlotButton, SIGNAL(clicked()), this, SLOT(_onGenericPlotButtonPressed()));

  this->_saveButton = new QPushButton(tr("Save data to file"));
  QObject::connect(_saveButton, SIGNAL(clicked()), this, SLOT(_onSaveButtonPressed()));


  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(_plotButton);
  button_box->addWidget(_genericPlotButton);
  button_box->addWidget(_saveButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_box);
  layout->addWidget(_dataTable);
  this->setLayout(layout);
}


void
IOSResultWidget::_onPlotButtonPressed()
{
  SpeciesSelectionDialog dialog(_ios_task_wrapper->getIOSTask()->getConfig().getModel());
  dialog.setWindowTitle(tr("IOS quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Rejected == dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  Application::getApp()->docTree()->addPlot(
        this->_ios_task_wrapper,
        new PlotItem(
          new IOSEMRETimeSeriesPlot(selected_species, _ios_task_wrapper->getIOSTask())));

  if (0 < selected_species.size()) {
    Application::getApp()->docTree()->addPlot(
          this->_ios_task_wrapper,
          new PlotItem(
            new IOSEMRECorrelationPlot(selected_species, _ios_task_wrapper->getIOSTask())));
  }

  Application::getApp()->docTree()->addPlot(
        this->_ios_task_wrapper,
        new PlotItem(
          new IOSEMREComparePlot(selected_species,_ios_task_wrapper->getIOSTask())));
}


void
IOSResultWidget::_onGenericPlotButtonPressed()
{
  // Show dialog
  GenericPlotDialog dialog(_ios_task_wrapper->getIOSTask()->getTimeSeries());
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
  Application::getApp()->docTree()->addPlot(_ios_task_wrapper, new PlotItem(figure));
}


void
IOSResultWidget::_onSaveButtonPressed()
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

  this->_ios_task_wrapper->getIOSTask()->getTimeSeries()->saveAsText(file);
  file.close();
}
