#include "steadystateview.hh"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

#include <fstream>
#include "../models/application.hh"
#include "../doctree/documenttree.hh"
#include "../doctree/plotitem.hh"
#include "steadystatespectrumplot.hh"
#include "../views/speciesselectiondialog.hh"
#include "utils/matexport.hh"


SteadyStateView::SteadyStateView(SteadyStateTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  // Update main-widget:
  taskStateChanged();
}


QWidget *
SteadyStateView::createResultWidget(TaskItem *task_item)
{
  return new SteadyStateResultWidget(static_cast<SteadyStateTaskWrapper *>(task_item));
}




SteadyStateResultWidget::SteadyStateResultWidget(SteadyStateTaskWrapper *task_wrapper, QWidget *parent)
  : QWidget(parent), ss_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Create state/concentrations table
  this->state_label = new QLabel("Concentrations");
  this->state_label->setFont(Application::getApp()->getH2Font());
  this->state_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->state_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // Headers
  QStringList headers;
  for (int i=0; i<this->ss_task_wrapper->getSteadyStateTask()->getConcentrations().rows(); i++)
  {
    headers.append(this->ss_task_wrapper->getSteadyStateTask()->getSpeciesName(i));
  }

  this->state_view = new QTableWidget();
  this->state_view->setColumnCount(this->ss_task_wrapper->getSteadyStateTask()->getConcentrations().rows());
  this->state_view->setRowCount(3);
  this->state_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->state_view->setHorizontalHeaderLabels(headers);
  this->state_view->setVerticalHeaderItem(0, new QTableWidgetItem("RE"));
  this->state_view->setVerticalHeaderItem(1, new QTableWidgetItem("EMRE"));
  this->state_view->setVerticalHeaderItem(2, new QTableWidgetItem("IOS"));

  this->lna_cov_label = new QLabel("Covariance matrix (LNA)");
  this->lna_cov_label->setFont(Application::getApp()->getH2Font());
  this->lna_cov_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->lna_cov_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // Create LNA covariance table:
  this->lna_covariance_view = new QTableWidget();
  this->lna_covariance_view->setColumnCount(this->ss_task_wrapper->getSteadyStateTask()->getLNACovariances().cols());
  this->lna_covariance_view->setRowCount(this->ss_task_wrapper->getSteadyStateTask()->getLNACovariances().rows());
  this->lna_covariance_view->setHorizontalHeaderLabels(headers);
  this->lna_covariance_view->setVerticalHeaderLabels(headers);
  this->lna_covariance_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  this->ios_cov_label = new QLabel("Covariance matrix (IOS)");
  this->ios_cov_label->setFont(Application::getApp()->getH2Font());
  this->ios_cov_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->ios_cov_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // Create IOS covariance table:
  this->ios_covariance_view = new QTableWidget();
  this->ios_covariance_view->setColumnCount(this->ss_task_wrapper->getSteadyStateTask()->getIOSCovariances().cols());
  this->ios_covariance_view->setRowCount(this->ss_task_wrapper->getSteadyStateTask()->getIOSCovariances().rows());
  this->ios_covariance_view->setHorizontalHeaderLabels(headers);
  this->ios_covariance_view->setVerticalHeaderLabels(headers);
  this->ios_covariance_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QVBoxLayout *state_layout = new QVBoxLayout();
  state_layout->addWidget(this->state_label);
  state_layout->addWidget(this->state_view);
  state_layout->addWidget(this->lna_cov_label);
  state_layout->addWidget(this->lna_covariance_view);
  state_layout->addWidget(this->ios_cov_label);
  state_layout->addWidget(this->ios_covariance_view);

  /*
  this->spec_label = new QLabel("Power spectrum (LNA)");
  this->spec_label->setFont(Application::getApp()->getH2Font());
  this->spec_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->spec_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  this->spectrum_view = new QTableView();
  this->spectrum_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->spectrum_view->setModel(this->ss_task_wrapper->getSpectrum());

  //this->spec_save_button = new QPushButton(tr("Save spectrum to file"));
  QObject::connect(this->spec_plot_button, SIGNAL(clicked()), this, SLOT(plotSpectrum()));
  //QObject::connect(this->spec_save_button, SIGNAL(clicked()), this, SLOT(saveSpectrum()));
  */
  this->spec_plot_button = new QPushButton(tr("Plot steady state statistics"));
  this->data_save_button = new QPushButton(tr("Save data to file"));
  QObject::connect(this->data_save_button, SIGNAL(clicked()), this, SLOT(saveData()));

  QHBoxLayout *button_layout = new QHBoxLayout();
  button_layout->addWidget(this->spec_plot_button);
  button_layout->addWidget(this->data_save_button);
  //button_layout->addWidget(this->spec_save_button);

  //QVBoxLayout *spectrum_layout = new QVBoxLayout();
  //spectrum_layout->addWidget(this->spec_label);
  //spectrum_layout->addWidget(this->spectrum_view);
  //spectrum_layout->addLayout(button_layout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(button_layout);
  layout->addLayout(state_layout);
  //layout->addLayout(spectrum_layout);
  this->setLayout(layout);

  // Connect signals:
  QObject::connect(this->ss_task_wrapper->getTask(), SIGNAL(stateChanged()),
                   this, SLOT(taskStateChanged()));

  // If task allready finished -> set values:
  if (this->ss_task_wrapper->getTask()->getState())
    this->setValues();
}



void
SteadyStateResultWidget::plotSpectrum()
{
  // Ask user to choose species to plot:
  SpeciesSelectionDialog dialog(ss_task_wrapper->getSteadyStateTask()->getConfig().getModel());
  dialog.setWindowTitle(tr("Steady State quick plot"));
  dialog.setTitle(tr("Select the species to plot."));
  if (QDialog::Rejected == dialog.exec()) { return; }
  QStringList selected_species = dialog.selectedSpecies();

  // Create plot
  Application::getApp()->docTree()->addPlot(
        this->ss_task_wrapper,
        new PlotItem(new SteadyStatePlot(
                       this->ss_task_wrapper->getSteadyStateTask(), selected_species)));
}


void
SteadyStateResultWidget::saveSpectrum()
{
  QString filename = QFileDialog::getSaveFileName(
        this, tr("Save as text..."), "", tr("Text Files (*.txt *.csv)"));

  if ("" == filename)
  {
    return;
  }

  QFile file(filename);

  if (! file.open(QIODevice::WriteOnly| QIODevice::Text))
  {
    QMessageBox box;
    box.setWindowTitle(tr("Cannot open file"));
    box.setText(tr("Cannot open file %1 for writing").arg(filename));
    box.exec();
  }

  this->ss_task_wrapper->getSteadyStateTask()->getSpectrum().saveAsText(file);
  file.close();
}


void
SteadyStateResultWidget::saveData()
{
  // First get file-name from user:
  QString selectedFilter;
  //QString filters = tr("Text Files (*.txt *.csv);;Matlab 5 Files (*.mat)";
  QString filters = tr("Text Files (*.txt *.csv)");
  QString filename = QFileDialog::getSaveFileName(
        this, tr("Save results as ..."), "", filters, &selectedFilter);
  if ("" == filename) { return; }

  if (tr("Text Files (*.txt *.csv)") == selectedFilter) {
    saveAsCSV(filename);
  } else if (tr("Matlab 5 Files (*.mat)") == selectedFilter) {
    saveAsMAT(filename);
  } else {
    QMessageBox::critical(
          0, tr("Can not save data."),
          tr("Can not save analysis result to file %1: Unknown type selected %2").arg(
            filename, selectedFilter));
  }
}

void
SteadyStateResultWidget::saveAsCSV(const QString &filename)
{
  QFile file(filename);
  if (! file.open(QIODevice::WriteOnly| QIODevice::Text)) {
    QMessageBox box;
    box.setWindowTitle(tr("Cannot open file"));
    box.setText(tr("Cannot open file %1 for writing").arg(filename));
    box.exec();
    return;
  }

  // Get data from task
  SteadyStateTask *task = this->ss_task_wrapper->getSteadyStateTask();
  Eigen::VectorXd conc = task->getConcentrations();
  Eigen::VectorXd emre = task->getEMRECorrections();
  Eigen::VectorXd ios  = task->getIOSCorrections();
  Eigen::MatrixXd lna_cov  = task->getLNACovariances();
  Eigen::MatrixXd ios_cov  = task->getIOSCovariances();
  QTextStream out(&file);

  // Dump header:
  out << "# First line: REs steady state.\n"
      << "# Second line: EMRE steady state.\n"
      << "# Remaining: IOS steady state covariance matrix.\n"
      << "\n";

  // Dump Species names/IDs:
  out << "# ";
  for (int i=0; i<conc.size(); i++) {
    out << task->getSpeciesName(i) << "\t";
  }
  out << "\n";

  // Dump concentrations:
  for(int i=0; i<conc.size(); i++) {
    out << conc(i) << "\t";
  }
  out << "\n";

  // Dump EMRE:
  for (int i=0; i<emre.size(); i++) {
    out << emre(i) + conc(i) << "\t";
  }
  out << "\n";

  // Dump IOS:
  for (int i=0; i<emre.size(); i++) {
    out << emre(i) + conc(i)  + ios(i) << "\t";
  }
  out << "\n";

  // Dump covariance:
  for(int i=0; i<lna_cov.rows(); i++) {
    for (int j=0; j<lna_cov.cols(); j++) {
      out << lna_cov(i,j) + ios_cov(i,j)<< "\t";
    }
    out << "\n";
  }

  // Close file:
  file.close();
}

void
SteadyStateResultWidget::saveAsMAT(const QString &filename) {
  std::fstream file(filename.toLocal8Bit().constData(),
                    std::fstream::out | std::fstream::binary);
  if (! file.is_open()) {
    QMessageBox box;
    box.setWindowTitle(tr("Cannot open file"));
    box.setText(tr("Cannot open file %1 for writing").arg(filename));
    box.exec();
    return;
  }

  // Get data from task
  SteadyStateTask *task = this->ss_task_wrapper->getSteadyStateTask();
  Eigen::VectorXd conc = task->getConcentrations();
  Eigen::VectorXd emre = task->getEMRECorrections();
  Eigen::VectorXd ios  = task->getIOSCorrections();
  Eigen::MatrixXd concentrations(3, conc.size()); concentrations << conc, emre, ios;
  Eigen::MatrixXd lna_cov  = task->getLNACovariances();
  Eigen::MatrixXd ios_cov  = task->getIOSCovariances();

  iNA::Utils::MatFile mat_file;
  mat_file.add("RE_con", conc);
  mat_file.add("EMRE_con", conc);
  mat_file.add("IOS_con", ios);
  mat_file.add("LNA_cov", lna_cov);
  mat_file.add("IOS_cov", ios_cov);
  mat_file.serialize(file);

  // Close file:
  file.close();
}


void
SteadyStateResultWidget::setValues()
{
  SteadyStateTask *task = this->ss_task_wrapper->getSteadyStateTask();
  Eigen::VectorXd conc = task->getConcentrations();
  Eigen::VectorXd emre = task->getEMRECorrections();
  Eigen::VectorXd iosemre = task->getIOSCorrections();
  Eigen::MatrixXd lna_cov  = task->getLNACovariances();
  Eigen::MatrixXd ios_cov  = task->getIOSCovariances();
  // Update state table:
  for (int i=0; i<conc.size(); i++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem(QString("%1").arg(conc(i)));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    this->state_view->setItem(0, i, item);

    item = new QTableWidgetItem(QString("%1").arg(conc(i) + emre(i)));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    this->state_view->setItem(1, i, item);

    item = new QTableWidgetItem(QString("%1").arg(conc(i) + emre(i) + iosemre(i)));
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    this->state_view->setItem(2, i, item);

    for (int j=0; j<conc.size(); j++)
    {
      item = new QTableWidgetItem(QString("%1").arg(lna_cov(j,i)));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      this->lna_covariance_view->setItem(j,i, item);

      item = new QTableWidgetItem(QString("%1").arg(lna_cov(j,i) + ios_cov(j,i)));
      item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      this->ios_covariance_view->setItem(j,i, item);
    }
  }

  // Set selection mode to avoid gray shaded color of values:
  this->state_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->state_view->setSelectionBehavior(QAbstractItemView::SelectItems);
  this->lna_covariance_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->lna_covariance_view->setSelectionBehavior(QAbstractItemView::SelectItems);
  this->ios_covariance_view->setSelectionMode(QAbstractItemView::ContiguousSelection);
  this->ios_covariance_view->setSelectionBehavior(QAbstractItemView::SelectItems);
}


void
SteadyStateResultWidget::taskStateChanged()
{
  if (Task::DONE == this->ss_task_wrapper->getTask()->getState())
  {
    this->setValues();
  }
}
