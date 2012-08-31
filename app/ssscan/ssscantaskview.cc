#include "ssscantaskview.hh"

#include <QVBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "ssscanplot.hh"


/* ********************************************************************************************* *
 * Implementation of TaskView, derived from TaskView:
 * ********************************************************************************************* */
SSScanTaskView::SSScanTaskView(SSScanTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
SSScanTaskView::createResultWidget(TaskItem *task_item)
{
  return new SSScanResultWidget(static_cast<SSScanTaskWrapper *>(task_item));
}



/* ********************************************************************************************* *
 * Implementation of ResultWidget, show the result of a Parameter scan.
 * ********************************************************************************************* */
SSScanResultWidget::SSScanResultWidget(SSScanTaskWrapper *task_wrapper, QWidget *parent):
  QWidget(parent), ssscan_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  this->dataTable = new QTableView();
  this->dataTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  this->tableWrapper = new TableWrapper(&(ssscan_task_wrapper->getSSScanTask()->getParameterScan()), this);
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
SSScanResultWidget::plotButtonPressed()
{
  std::stringstream unit_str;
  this->ssscan_task_wrapper->getSSScanTask()->getSpeciesUnit().dump(unit_str, true);

  QString concentration_unit(unit_str.str().c_str());
  QString time_unit("a.u.");

  // Add LNA parameter plot:
  Application::getApp()->docTree()->addPlot(
        this->ssscan_task_wrapper,
        new PlotItem(
          new ParameterScanPlot(this->ssscan_task_wrapper->getSSScanTask()->numSpecies(),
                                &(this->ssscan_task_wrapper->getSSScanTask()->getParameterScan()),
                                concentration_unit, time_unit)));
  // Add IOS parameter plot
  Application::getApp()->docTree()->addPlot(
        this->ssscan_task_wrapper,
        new PlotItem(
          new ParameterScanIOSPlot(this->ssscan_task_wrapper->getSSScanTask()->numSpecies(),
                                &(this->ssscan_task_wrapper->getSSScanTask()->getParameterScan()),
                                concentration_unit, time_unit)));

}


void
SSScanResultWidget::saveButtonPressed()
{
}
