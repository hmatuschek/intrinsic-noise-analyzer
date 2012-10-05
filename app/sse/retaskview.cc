#include "retaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "replot.hh"
#include "../views/timeseriesplotwizard.hh"


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
REResultWidget::plotButtonPressed()
{
  // Show dialog
  TimeSeriesPlotDialog dialog(re_task_wrapper->getRETask()->getTimeSeries());
  // on cancel -> return
  if (QDialog::Rejected == dialog.exec()) { return; }

  // Get species unit
  std::stringstream unit_str;
  this->re_task_wrapper->getRETask()->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());

  // Get time unit
  unit_str.str("");
  this->re_task_wrapper->getRETask()->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());

  // Create plot figure
  Plot::Figure *figure = new Plot::Figure("Mean concentrations (RE)");
  figure->getAxis()->setXLabel(tr("time [%1]").arg(time_unit));
  figure->getAxis()->setYLabel(tr("concentrations [%1]").arg(species_unit));

  // Iterate over all graphs of the configured plot:
  for (size_t i=0; i<dialog.numGraphs(); i++) {
    figure->getAxis()->addGraph(dialog.graph(i).create(figure->getStyle(i)));
  }

  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(this->re_task_wrapper, new PlotItem(figure));
}


void
REResultWidget::saveButtonPressed()
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

  this->re_task_wrapper->getRETask()->getTimeSeries()->saveAsText(file);
  file.close();
}
