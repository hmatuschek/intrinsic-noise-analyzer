#include "ssaview.hh"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "../views/speciesselectiondialog.hh"
#include "ssaplot.hh"



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
SSATaskView::createResultWidget(TaskItem *task_item)
{
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
  this->plot_button = new QPushButton(tr("Plot statistics"));
  QObject::connect(this->plot_button, SIGNAL(clicked()), this, SLOT(showPlot()));

  // "Save as text..." button.
  this->save_button = new QPushButton(tr("Save data to file"));
  QObject::connect(this->save_button, SIGNAL(clicked()), this, SLOT(saveData()));

  // Layout-box for buttons:
  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addWidget(plot_button);
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
  QStringList selected_species = dialog.getSelectedSpecies();

  // Simply construct and add plot to task:
  Application::getApp()->docTree()->addPlot(
        this->ssa_task_wrapper,
        new PlotItem(new SSAPlot(selected_species, this->ssa_task_wrapper->getSSATask())));
  Application::getApp()->docTree()->addPlot(
        this->ssa_task_wrapper,
        new PlotItem(new SSACorrelationPlot(selected_species, this->ssa_task_wrapper->getSSATask())));
}


void
SSAResultWidget::saveData()
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

  this->ssa_task_wrapper->getSSATask()->getTimeSeries().saveAsText(file);
  file.close();
}

