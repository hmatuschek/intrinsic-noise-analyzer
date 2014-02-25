#include "ssaparamscanwizard.hh"
#include "../models/application.hh"
#include "../models/scopeitemmodel.hh"
#include "trafo/constantfolder.hh"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QCheckBox>
#include <QGroupBox>

using namespace iNA;

SSAParamScanWizard::SSAParamScanWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("Parameter Scan (SSA)");

  this->setPage(SSAParamScanWizard::MODEL_SELECTION_PAGE, new SSAParamScanModelSelectionPage(this));
  this->setPage(SSAParamScanWizard::SCAN_CONFIG_PAGE, new SSAParamScanConfigPage(this));
  this->setPage(SSAParamScanWizard::ENGINE_SELECTION_PAGE, new SSAParamScanEngineSelectionPage(this));
  this->setPage(SSAParamScanWizard::SUMMARY_PAGE, new SSAParamScanSummaryPage(this));
  this->page(SSAParamScanWizard::SUMMARY_PAGE)->setFinalPage(true);
}

GeneralTaskConfig &
SSAParamScanWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation of model selection page:
 * ********************************************************************************************* */
SSAParamScanModelSelectionPage::SSAParamScanModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Time-averaged Parameter Scan (SSA)"));
  this->setSubTitle(tr("Select a model for parameter scan"));

}


bool
SSAParamScanModelSelectionPage::validatePage()
{
  // Try to create SSA model from SBML document
  try {
    // Forward call to parent implementation:
    if (! ModelSelectionWizardPage::validatePage())
      return false;
  } catch (Exception err) {
    // Simply show a warning and done.
    QMessageBox::warning(0, tr("Cannot construct analysis from model: "), err.what());
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of the engine selection page:
 * ********************************************************************************************* */
SSAParamScanEngineSelectionPage::SSAParamScanEngineSelectionPage(GeneralTaskWizard *parent)
  : EngineWizardPage(parent, false)
{
  this->setTitle(tr("Time-averaged Parameter Scan (SSA)"));
  setSubTitle(tr("Select engine for propensity evaluation"));
}

/* ********************************************************************************************* *
 * Config page
 * ********************************************************************************************* */
SSAParamScanConfigPage::SSAParamScanConfigPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Time-averaged Parameter Scan (SSA)"));
  this->setSubTitle(tr("Configure parameter scan"));

  this->t_transient = new QLineEdit(); this->t_transient->setText("1");
  this->t_transient->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *t_transient_val = new QIntValidator(t_transient); t_transient_val->setBottom(1);
  this->t_transient->setValidator(t_transient_val);
  this->registerField("t_transient", this->t_transient);

  this->timestep = new QLineEdit(); this->timestep->setText("0.1");
  this->timestep->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *timestep_val = new QDoubleValidator(timestep); timestep_val->setBottom(1);
  this->timestep->setValidator(timestep_val);
  this->registerField("timestep", this->timestep);

  t_max = new QLineEdit(); t_max->setText("1e9");
  t_max->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *t_max_val = new QDoubleValidator(t_max); t_max_val->setBottom(0);
  t_max->setValidator(t_max_val);

  this->t_transient->setToolTip("Estimated simulation time to reach steady state.");
  this->t_max->setToolTip("Maximum simulation time.");
  this->timestep->setToolTip("Time step of trajectory sampling.");

  p_select = new QComboBox();
  p_select->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QObject::connect(p_select, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshParamRange(int)));

  p_min = new QLineEdit("0.0");
  p_min->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *p_min_val = new QDoubleValidator(p_min);
  p_min->setValidator(p_min_val);
  registerField("p_min", p_min);

  p_max = new QLineEdit("1.0");
  p_max->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *p_max_val = new QDoubleValidator(p_max);
  p_max->setValidator(p_max_val);
  p_max->setToolTip("Maximum parameter value.");
  registerField("p_max", p_max);

  p_num = new QLineEdit("100");
  p_num->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *p_num_val = new QIntValidator(p_num); p_num_val->setBottom(1);
  p_num->setValidator(p_num_val);
  p_num->setToolTip("Number of steps taken between minimum and maximum parameter value.");
  registerField("p_num", p_num);

  thread_count = new QSpinBox();
  thread_count->setMinimum(1);
  thread_count->setMaximum(OpenMP::getMaxThreads());
  thread_count->setValue(OpenMP::getMaxThreads());
  thread_count->setToolTip("Number of threads used for parallel stochastic simulation.");
  this->registerField("thread_count", thread_count);
  if (1 == OpenMP::getMaxThreads())
    thread_count->setEnabled(false);


  QGroupBox *param_box = new QGroupBox(tr("Parameter scan"));
  QFormLayout *param_layout = new QFormLayout();
  param_layout->addRow(tr("Parameter"), p_select);
  param_layout->addRow(tr("Min. value"), p_min);
  param_layout->addRow(tr("Max. value"), p_max);
  param_layout->addRow(tr("Steps"), p_num);
  param_box->setLayout(param_layout);

  QGroupBox *mp_box = new QGroupBox(tr("Parallel process"));
  QFormLayout *mp_layout = new QFormLayout();
  mp_layout->addRow(tr("Thread count"), thread_count);
  mp_box->setLayout(mp_layout);

  QGroupBox *ss_box = new QGroupBox(tr("Averaging"));
  QFormLayout *ss_layout = new QFormLayout();
  ss_layout->addRow(tr("Transient time"), t_transient);
  ss_layout->addRow(tr("Max. simulation time"), t_max);
  ss_layout->addRow(tr("Average taken every"), timestep);
  ss_box->setLayout(ss_layout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(param_box);
  layout->addWidget(ss_box);
  layout->addWidget(mp_box);
  setLayout(layout);
}

void
SSAParamScanConfigPage::refreshParamRange(int)
{
  // Get the config:
  SSAParamScanWizard *wizard = static_cast<SSAParamScanWizard *>(this->wizard());
  SSAParamScanTask::Config &config = wizard->getConfigCast<SSAParamScanTask::Config>();

  // Get parameter
  QString idp = p_select->currentText();
  if (! config.getModel()->hasParameter(idp.toStdString())) { return; }
  iNA::Ast::Parameter * parameter = config.getModel()->getParameter(idp.toStdString());

  // Set range (if parameter has an initial value assigned)
  if(parameter->hasValue())
  {
    GiNaC::ex value = parameter->getValue();
    // Fold constants in parameter initial value:
    iNA::Trafo::ConstantFolder folder(*(config.getModel()));
    value = folder.apply(value);
    if (GiNaC::is_a<GiNaC::numeric>(value)) {
      double val = Eigen::ex2double(value);
      p_min->setText(QString("%1").arg(val));
      p_max->setText(QString("%1").arg(1.1*val));
    }
  }
}

void
SSAParamScanConfigPage::initializePage()
{
  // Get the config:
  SSAParamScanWizard *wizard = static_cast<SSAParamScanWizard *>(this->wizard());
  SSAParamScanTask::Config &config = wizard->getConfigCast<SSAParamScanTask::Config>();

  // Init parameter list:
  p_select->clear();
  p_select->addItems(ScopeItemModel::collectIdentifiers(
                       *(config.getModel()), ScopeItemModel::SELECT_PARAMETERS));
  if (p_select->model()->rowCount() > 0) { p_select->setCurrentIndex(0); }

}


bool
SSAParamScanConfigPage::validatePage()
{
  // Get the wizard:
  SSAParamScanWizard *wizard = static_cast<SSAParamScanWizard *>(this->wizard());
  SSAParamScanTask::Config &config = wizard->getConfigCast<SSAParamScanTask::Config>();

  bool ok;
  config.setTransientTime(t_transient->text().toDouble(&ok));
  config.setMaxTime(t_max->text().toDouble(&ok));
  config.setTimeStep(timestep->text().toDouble(&ok));
  config.setNumThreads(thread_count->text().toDouble(&ok));

  std::string pid = p_select->currentText().toStdString();
  // If model does not exists:
  if (! config.getModel()->hasParameter(pid)) { return false; }
  config.setParameter(config.getModel()->getParameter(pid));

  config.setStartValue(p_min->text().toDouble(&ok));
  config.setEndValue(p_max->text().toDouble(&ok));
  config.setSteps(p_num->text().toInt(&ok));

  return ok;
}





/* ********************************************************************************************* *
 * Implementation of summary page:
 * ********************************************************************************************* */
SSAParamScanSummaryPage::SSAParamScanSummaryPage(QWidget *parent)
  : QWizardPage(parent)
{

  this->setTitle("Time-averaged Parameter Scan (SSA)");
  this->setSubTitle("Summary");

  this->model_name = new QLabel();
  this->model_name->setTextFormat(Qt::LogText);

  this->param = new QLabel();
  this->param->setTextFormat(Qt::LogText);
  this->param->setWordWrap(true);

  this->memory = new QLabel();
  this->memory->setTextFormat(Qt::LogText);

  this->method = new QLabel();
  this->method->setTextFormat(Qt::LogText);

  QFormLayout *layout = new QFormLayout();
  layout->addRow("Model:", this->model_name);
  layout->addRow("Method:", this->method);
  layout->addRow("Selected parameter:", this->param);
  layout->addRow("Approx. memory used:", this->memory);
  this->setLayout(layout);



}


void
SSAParamScanSummaryPage::initializePage()
{
  SSAParamScanWizard *wizard = static_cast<SSAParamScanWizard *>(this->wizard());
  SSAParamScanTask::Config &config = wizard->getConfigCast<SSAParamScanTask::Config>();

  this->model_name->setText(config.getModelDocument()->getModel().getName().c_str());

  this->param->setText(QString((config.getParameter().hasName() ? config.getParameter().getName() : config.getParameter().getIdentifier()).c_str()));

  this->method->setText("SSA");

  QString mem_str("%1 MB");
  int N = config.getModel()->numSpecies();
  this->memory->setText(mem_str.arg(double(8*(2*N+N*(N-1)))/126976.));

}

