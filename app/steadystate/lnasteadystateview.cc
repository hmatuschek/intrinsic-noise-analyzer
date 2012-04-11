#include "lnasteadystateview.hh"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>


#include "../application.hh"
#include "../doctree/plotitem.hh"
#include "steadystatespectrumplot.hh"


LNASteadyStateView::LNASteadyStateView(LNASteadyStateTaskWrapper *task_wrapper, QWidget *parent)
  : TaskView(task_wrapper, parent)
{
  this->setResultWidget(new LNASteadyStateResultWidget(task_wrapper, 0));
}


LNASteadyStateResultWidget::LNASteadyStateResultWidget(LNASteadyStateTaskWrapper *task_wrapper, QWidget *parent)
  : QWidget(parent), ss_task_wrapper(task_wrapper)
{
  this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  this->setBackgroundRole(QPalette::Window);

  // Create state/concentrations table
  this->state_label = new QLabel("Concentrations (RE / EMRE)");
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
  this->state_view->setRowCount(2);
  this->state_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->state_view->setHorizontalHeaderLabels(headers);
  this->state_view->setVerticalHeaderItem(0, new QTableWidgetItem("RE"));
  this->state_view->setVerticalHeaderItem(1, new QTableWidgetItem("EMRE"));

  this->cov_label = new QLabel("Covariance matrix (LNA)");
  this->cov_label->setFont(Application::getApp()->getH2Font());
  this->cov_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->cov_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // Create covariance table:
  this->covariance_view = new QTableWidget();
  this->covariance_view->setColumnCount(this->ss_task_wrapper->getSteadyStateTask()->getCovariances().cols());
  this->covariance_view->setRowCount(this->ss_task_wrapper->getSteadyStateTask()->getCovariances().rows());
  this->covariance_view->setHorizontalHeaderLabels(headers);
  this->covariance_view->setVerticalHeaderLabels(headers);
  this->covariance_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QVBoxLayout *state_layout = new QVBoxLayout();
  state_layout->addWidget(this->state_label);
  state_layout->addWidget(this->state_view);
  state_layout->addWidget(this->cov_label);
  state_layout->addWidget(this->covariance_view);

  this->spec_label = new QLabel("Power spectrum (LNA)");
  this->spec_label->setFont(Application::getApp()->getH2Font());
  this->spec_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->spec_label->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  this->spectrum_view = new QTableView();
  this->spectrum_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->spectrum_view->setModel(this->ss_task_wrapper->getSpectrum());

  this->spec_plot_button = new QPushButton(tr("Plot steady state statistics"));
  this->spec_save_button = new QPushButton(tr("Save spectrum to file"));
  this->data_save_button = new QPushButton(tr("Save data to file"));
  QObject::connect(this->spec_plot_button, SIGNAL(clicked()), this, SLOT(plotSpectrum()));
  //QObject::connect(this->spec_save_button, SIGNAL(clicked()), this, SLOT(saveSpectrum()));
  QObject::connect(this->data_save_button, SIGNAL(clicked()), this, SLOT(saveData()));

  QHBoxLayout *button_layout = new QHBoxLayout();
  button_layout->addWidget(this->spec_plot_button);
  button_layout->addWidget(this->data_save_button);
  //button_layout->addWidget(this->spec_save_button);

  QVBoxLayout *spectrum_layout = new QVBoxLayout();
  //spectrum_layout->addWidget(this->spec_label);
  //spectrum_layout->addWidget(this->spectrum_view);
  spectrum_layout->addLayout(button_layout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(state_layout);
  layout->addLayout(spectrum_layout);
  this->setLayout(layout);

  // Connect signals:
  QObject::connect(this->ss_task_wrapper->getTask(), SIGNAL(stateChanged()),
                   this, SLOT(taskStateChanged()));

  // If task allready finished -> set values:
  if (this->ss_task_wrapper->getTask()->getState())
    this->setValues();
}



void
LNASteadyStateResultWidget::plotSpectrum()
{
  Application::getApp()->docTree()->addPlot(
        this->ss_task_wrapper,
        new PlotItem(new SteadyStatePlot(this->ss_task_wrapper->getSteadyStateTask())));

//  Application::getApp()->docTree()->addPlot(
//        this->ss_task_wrapper,
//        new PlotItem(new SteadyStateSpectrumPlot(
//                          this->ss_task_wrapper->getSteadyStateTask()->getSpectrum(),
//                          this->ss_task_wrapper->getSteadyStateTask()->getModel()->getConcentrationUnit(),
//                          this->ss_task_wrapper->getSteadyStateTask()->getModel()->getTimeUnit())));
}


void
LNASteadyStateResultWidget::saveSpectrum()
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
LNASteadyStateResultWidget::saveData()
{
  // First get file-name from user:
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

  // Get data from task
  LNASteadyStateTask *task = this->ss_task_wrapper->getSteadyStateTask();
  Eigen::VectorXd conc = task->getConcentrations();
  Eigen::VectorXd emre = task->getEMRECorrections();
  Eigen::MatrixXd cov  = task->getCovariances();

  QTextStream out(&file);

  // Dump header:
  out << "# First line: REs steady state.\n"
      << "# Second line: EMRE steady state.\n"
      << "# Remaining: LNA steady state covariance matrix.\n"
      << "\n";

  // Dump Species names/IDs:
  out << "# ";
  for (int i=0; i<conc.size(); i++)
  {
    out << task->getSpeciesName(i) << "\t";
  }
  out << "\n";

  // Dump concentrations:
  for(int i=0; i<conc.size(); i++)
  {
    out << conc(i) << "\t";
  }
  out << "\n";

  // Dump EMRE:
  for (int i=0; i<emre.size(); i++)
  {
    out << emre(i) + conc(i) << "\t";
  }
  out << "\n";

  // Dump covariance:
  for(int i=0; i<cov.rows(); i++) {
    for (int j=0; j<cov.cols(); j++) {
      out << cov(i,j) << "\t";
    }
    out << "\n";
  }

  // Close file:
  file.close();
}


void
LNASteadyStateResultWidget::setValues()
{
  LNASteadyStateTask *task = this->ss_task_wrapper->getSteadyStateTask();
  Eigen::VectorXd conc = task->getConcentrations();
  Eigen::VectorXd emre = task->getEMRECorrections();
  Eigen::MatrixXd cov  = task->getCovariances();

  // Update state table:
  for (int i=0; i<conc.size(); i++)
  {
    QTableWidgetItem *item;

    item = new QTableWidgetItem(QString("%1").arg(conc(i)));
    item->setFlags(Qt::ItemIsSelectable);
    this->state_view->setItem(0, i, item);

    item = new QTableWidgetItem(QString("%1").arg(conc(i) + emre(i)));
    item->setFlags(Qt::ItemIsSelectable);
    this->state_view->setItem(1, i, item);

    for (int j=0; j<conc.size(); j++)
    {
      item = new QTableWidgetItem(QString("%1").arg(cov(j,i)));
      item->setFlags(Qt::ItemIsSelectable);
      this->covariance_view->setItem(j,i, item);
    }
  }
}


void
LNASteadyStateResultWidget::taskStateChanged()
{
  if (Task::DONE == this->ss_task_wrapper->getTask()->getState())
  {
    this->setValues();
  }
}
