#include "iostaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "iosplot.hh"


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
  QWidget(parent), ios_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  this->dataTable = new QTableView();
  this->dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  this->tableWrapper = new TableWrapper(ios_task_wrapper->getIOSTask()->getTimeSeries(), this);
  dataTable->setModel(this->tableWrapper);

  this->plotButton = new QPushButton(tr("Plot statistics"));
  QObject::connect(this->plotButton, SIGNAL(clicked()), this, SLOT(plotButtonPressed()));

  this->saveButton = new QPushButton(tr("Save data to file"));
  QObject::connect(this->saveButton, SIGNAL(clicked()), this, SLOT(saveButtonPressed()));


  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(this->plotButton);
  button_box->addWidget(this->saveButton);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->dataTable);
  layout->addLayout(button_box);
  this->setLayout(layout);
}


void
IOSResultWidget::plotButtonPressed()
{
  std::stringstream unit_str;
  this->ios_task_wrapper->getIOSTask()->getSpeciesUnit().dump(unit_str, true);
  QString concentration_unit(unit_str.str().c_str());

  unit_str.str("");
  this->ios_task_wrapper->getIOSTask()->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());


  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSLNATimeSeriesPlot(this->ios_task_wrapper->getIOSTask()->getSelectedSpecies().size(),
                                this->ios_task_wrapper->getIOSTask()->getTimeSeries(),
                                concentration_unit, time_unit)));

  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSLNACorrelationPlot(this->ios_task_wrapper->getIOSTask(), time_unit)));

  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSEMRETimeSeriesPlot(this->ios_task_wrapper->getIOSTask()->getSelectedSpecies().size(),
                                    this->ios_task_wrapper->getIOSTask()->getTimeSeries(),
                                    concentration_unit, time_unit)));


  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSEMRECorrelationPlot(this->ios_task_wrapper->getIOSTask(), time_unit)));

  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSEMREComparePlot(this->ios_task_wrapper->getIOSTask()->getSelectedSpecies().size(),
                                 this->ios_task_wrapper->getIOSTask()->getTimeSeries(),
                                 concentration_unit, time_unit)));

}


void
IOSResultWidget::saveButtonPressed()
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

  this->ios_task_wrapper->getIOSTask()->getTimeSeries()->saveAsText(file);
  file.close();
}
