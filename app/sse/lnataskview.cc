#include "lnataskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "lnaplot.hh"


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
LNATaskView::createResultWidget(TaskItem *task_item)
{
  return new LNAResultWidget(static_cast<LNATaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of LNAResultWidget, show the result of a LNA (SSE) analysis.
 * ********************************************************************************************* */
LNAResultWidget::LNAResultWidget(LNATaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), lna_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  this->dataTable = new QTableView();
  this->dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  this->tableWrapper = new TableWrapper(lna_task_wrapper->getLNATask()->getTimeSeries(), this);
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
LNAResultWidget::plotButtonPressed()
{
  std::stringstream unit_str;
  this->lna_task_wrapper->getLNATask()->getSpeciesUnit().dump(unit_str, true);
  QString concentration_unit(unit_str.str().c_str());

  unit_str.str("");
  this->lna_task_wrapper->getLNATask()->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());


  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(
        this->lna_task_wrapper,
        new PlotItem(
          new LNATimeSeriesPlot(this->lna_task_wrapper->getLNATask()->getSelectedSpecies().size(),
                                this->lna_task_wrapper->getLNATask()->getTimeSeries(),
                                concentration_unit, time_unit)));

  // Add correlation coefficient plot (if there are more than one species selected).
  if (1 < this->lna_task_wrapper->getLNATask()->getSelectedSpecies().size()) {
    Application::getApp()->docTree()->addPlot(
          this->lna_task_wrapper,
          new PlotItem(
            new LNACorrelationPlot(this->lna_task_wrapper->getLNATask(),
                                   time_unit)));
  }


  // Add EMRE plot:
//  Application::getApp()->docTree()->addPlot(
//        this->lna_task_wrapper,
//        new PlotItem(
//          new EMRETimeSeriesPlot(this->lna_task_wrapper->getLNATask()->getSelectedSpecies().size(),
//                                 this->lna_task_wrapper->getLNATask()->getTimeSeries(),
//                                 concentration_unit, time_unit)));
}


void
LNAResultWidget::saveButtonPressed()
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
    box.setWindowTitle(tr("Cannot open file"));
    box.setText(tr("Cannot open file %1 for writing").arg(filename));
    box.exec();
  }

  this->lna_task_wrapper->getLNATask()->getTimeSeries()->saveAsText(file);
  file.close();
}
