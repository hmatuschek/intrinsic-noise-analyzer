#include "paramscanwizard.hh"
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

ParamScanWizard::ParamScanWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("System Size Expansion");

  this->setPage(ParamScanWizard::MODEL_SELECTION_PAGE, new ParamScanModelSelectionPage(this));
  this->setPage(ParamScanWizard::ENGINE_SELECTION_PAGE, new ParamScanEngineSelectionPage(this));
  this->setPage(ParamScanWizard::SCAN_CONFIG_PAGE, new ParameterScanConfigPage(this));
  this->setPage(ParamScanWizard::SUMMARY_PAGE, new ParamScanSummaryPage(this));
  this->page(ParamScanWizard::SUMMARY_PAGE)->setFinalPage(true);
}

GeneralTaskConfig &
ParamScanWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation of model selection page:
 * ********************************************************************************************* */
ParamScanModelSelectionPage::ParamScanModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Parameter scan (SSE)"));
  this->setSubTitle(tr("Select a model for parameter scan"));


  // Append method selection radio buttons to the wizard page.
  _re_button  = new QRadioButton("Rate Equations (REs)");
  _lna_button = new QRadioButton("Linear Noise Approximation (LNA)");
  _ios_button = new QRadioButton("Higher Order Approximations (EMREs && IOS)");

  _re_button->setToolTip("Deterministic analysis using macroscopic rate equations.");
  _lna_button->setToolTip("Analysis yielding the fluctuations about the macroscopic concentrations.");
  _ios_button->setToolTip("Analysis yielding corrections beyond the RE and LNA analysis.");

  QVBoxLayout *box =  new QVBoxLayout();
  box->addWidget(_re_button);
  box->addWidget(_lna_button);
  box->addWidget(_ios_button);

  QGroupBox *button_grp = new QGroupBox("Analysis method");
  button_grp->setLayout(box);
  dynamic_cast<QBoxLayout *>(layout())->addSpacing(20);
  layout()->addWidget(button_grp);

  _re_button->setChecked(true);
}


bool
ParamScanModelSelectionPage::validatePage()
{
  // First set the analysis method, this determines which model will be instantiated:
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  ParamScanTask::Config &config = wizard->getConfigCast< ParamScanTask::Config >();
  if (_re_button->isChecked())
    config.setMethod(ParamScanTask::Config::RE_ANALYSIS);
  else if (_lna_button->isChecked())
    config.setMethod(ParamScanTask::Config::LNA_ANALYSIS);
  else
    config.setMethod(ParamScanTask::Config::IOS_ANALYSIS);

  // Try to create LNA model from SBML document
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
 * Config page
 * ********************************************************************************************* */
ParameterScanConfigPage::ParameterScanConfigPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Parameter scan (SSE)"));
  this->setSubTitle(tr("Configure parameter scan"));

  this->n_iter = new QLineEdit(); this->n_iter->setText("100");
  this->n_iter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *n_iter_val = new QIntValidator(); n_iter_val->setBottom(1);
  this->n_iter->setValidator(n_iter_val);
  this->registerField("n_iter", this->n_iter);

  this->epsilon = new QLineEdit(); this->epsilon->setText("1e-9");
  this->epsilon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *epsilon_val = new QDoubleValidator(0); epsilon_val->setBottom(1);
  this->epsilon->setValidator(epsilon_val);
  this->registerField("epsilon", this->epsilon);

  t_max = new QLineEdit(); t_max->setText("1e9");
  t_max->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *t_max_val = new QDoubleValidator(0); t_max_val->setBottom(0);
  t_max->setValidator(t_max_val);

  this->n_iter->setToolTip("Maximum number of iterations used by the Newton-Rapson method.");
  this->t_max->setToolTip("iNA makes use of the ODE integrator LSODA in case the Newton-Raphson method fails. \n"
                          "The option specifies the maximum time by which the system should have reached steady state.");
  this->epsilon->setToolTip("Accuracy of the Newton-Rapson method.");

  p_select = new QComboBox();
  p_select->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QObject::connect(p_select, SIGNAL(currentIndexChanged(int)), this, SLOT(refreshParamRange(int)));

  p_min = new QLineEdit("0.0");
  p_min->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *p_min_val = new QDoubleValidator();
  p_min->setValidator(p_min_val);
  p_min->setToolTip("Minimum parameter value.");
  registerField("p_min", p_min);

  p_max = new QLineEdit("1.0");
  p_max->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *p_max_val = new QDoubleValidator();
  p_max->setValidator(p_max_val);
  p_max->setToolTip("Maximum parameter value.");
  registerField("p_max", p_max);

  p_num = new QLineEdit("100");
  p_num->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *p_num_val = new QIntValidator(); p_num_val->setBottom(1);
  p_num->setValidator(p_num_val);
  p_num->setToolTip("Number of steps taken from minimum to maximum parameter value.");
  registerField("p_num", p_num);

  QSpinBox *thread_count = new QSpinBox();
  thread_count->setMinimum(1);
  thread_count->setMaximum(OpenMP::getMaxThreads());
  thread_count->setValue(OpenMP::getMaxThreads());
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

  QGroupBox *ss_box = new QGroupBox(tr("Steady state solver"));
  QFormLayout *ss_layout = new QFormLayout();
  ss_layout->addRow(tr("Precision"), epsilon);
  ss_layout->addRow(tr("Max. iterations"), n_iter);
  ss_layout->addRow(tr("Max. integration time"), t_max);
  ss_box->setLayout(ss_layout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(param_box);
  //layout->addWidget(mp_box);
  layout->addWidget(ss_box);
  setLayout(layout);
}

void
ParameterScanConfigPage::refreshParamRange(int)
{
  // Get the config:
  ParamScanWizard *wizard = static_cast<ParamScanWizard *>(this->wizard());
  ParamScanTask::Config &config = wizard->getConfigCast<ParamScanTask::Config>();

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
      double val = GiNaC::ex_to<GiNaC::numeric>(value).to_double();
      p_min->setText(QString("%1").arg(val));
      p_max->setText(QString("%1").arg(1.1*val));
    }
  }
}

void
ParameterScanConfigPage::initializePage()
{
  // Get the config:
  ParamScanWizard *wizard = static_cast<ParamScanWizard *>(this->wizard());
  ParamScanTask::Config &config = wizard->getConfigCast<ParamScanTask::Config>();

  // Init parameter list:
  p_select->clear();
  p_select->addItems(ScopeItemModel::collectIdentifiers(
                       *(config.getModel()), ScopeItemModel::SELECT_PARAMETERS));
  if (p_select->model()->rowCount() > 0) { p_select->setCurrentIndex(0); }

}


bool
ParameterScanConfigPage::validatePage()
{
  // Get the wizard:
  ParamScanWizard *wizard = static_cast<ParamScanWizard *>(this->wizard());
  ParamScanTask::Config &config = wizard->getConfigCast<ParamScanTask::Config>();

  bool ok;
  config.setEpsilon(epsilon->text().toDouble(&ok));
  config.setMaxIterations(n_iter->text().toInt(&ok));
  config.setMaxTimeStep(t_max->text().toDouble(&ok));

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
 * Implementation of the engine selection page:
 * ********************************************************************************************* */
ParamScanEngineSelectionPage::ParamScanEngineSelectionPage(GeneralTaskWizard *parent)
  : EngineWizardPage(parent, false)
{

  this->setTitle(tr("Parameter scan (SSE)"));
  this->setSubTitle(tr("Select engine for model evaluation"));
}


/* ********************************************************************************************* *
 * Implementation of summary page:
 * ********************************************************************************************* */
ParamScanSummaryPage::ParamScanSummaryPage(QWidget *parent)
  : QWizardPage(parent)
{

  this->setTitle("Parameter scan (SSE)");
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
ParamScanSummaryPage::initializePage()
{
  ParamScanWizard *wizard = static_cast<ParamScanWizard *>(this->wizard());
  ParamScanTask::Config &config = wizard->getConfigCast<ParamScanTask::Config>();

  this->model_name->setText(config.getModelDocument()->getModel().getName().c_str());

  this->param->setText(QString((config.getParameter().hasName() ? config.getParameter().getName() : config.getParameter().getIdentifier()).c_str()));

  QString mem_str("%1 MB");
  int N = config.getModel()->numSpecies();
  this->memory->setText(mem_str.arg(double(8*(2*N+N*(N-1)))/126976.));

  switch(config.getMethod())
  {
      case ParamScanTask::Config::RE_ANALYSIS:
          method->setText("RE"); break;
      case ParamScanTask::Config::LNA_ANALYSIS:
          method->setText("LNA"); break;
      case ParamScanTask::Config::IOS_ANALYSIS:
          method->setText("IOS"); break;
      case ParamScanTask::Config::UNDEFINED_ANALYSIS:
          method->setText("undefined"); break;
  }


}

