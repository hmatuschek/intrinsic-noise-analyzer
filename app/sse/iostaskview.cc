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
#include "replotdialog.hh"


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
  layout->addLayout(button_box);
  layout->addWidget(this->dataTable);
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

  SSEPlotDialog dialog(ios_task_wrapper->getIOSTask()->getConfig().getModel());
  dialog.setWindowTitle(tr("IOS quick plot dialog"));
  dialog.setTitle(tr("Select the species to plot."));

  if (QDialog::Rejected == dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSEMRETimeSeriesPlot(selected_species, ios_task_wrapper->getIOSTask())));

  if (0 < selected_species.size()) {
    Application::getApp()->docTree()->addPlot(
          this->ios_task_wrapper,
          new PlotItem(
            new IOSEMRECorrelationPlot(selected_species, ios_task_wrapper->getIOSTask())));
  }

  Application::getApp()->docTree()->addPlot(
        this->ios_task_wrapper,
        new PlotItem(
          new IOSEMREComparePlot(selected_species,ios_task_wrapper->getIOSTask())));
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
