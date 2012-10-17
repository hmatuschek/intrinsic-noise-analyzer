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
#include "../views/speciesselectiondialog.hh"


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
  layout->addLayout(button_box);
  layout->addWidget(this->dataTable);
  this->setLayout(layout);
}


void
LNAResultWidget::plotButtonPressed()
{
  SpeciesSelectionDialog dialog(lna_task_wrapper->getLNATask()->getConfig().getModel());
  dialog.setWindowTitle(tr("LNA quick plot"));
  dialog.setTitle(tr("Select the species to plot."));

  if (QDialog::Rejected == dialog.exec()) { return; }
  QStringList selected_species = dialog.getSelectedSpecies();

  // Add timeseries plot:
  Application::getApp()->docTree()->addPlot(
        this->lna_task_wrapper,
        new PlotItem(new LNATimeSeriesPlot(selected_species, lna_task_wrapper->getLNATask())));

  // Add correlation coefficient plot (if there are more than one species selected).
  if (1 < selected_species.size()) {
    Application::getApp()->docTree()->addPlot(
          this->lna_task_wrapper,
          new PlotItem(
            new LNACorrelationPlot(selected_species, lna_task_wrapper->getLNATask())));
  }
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
