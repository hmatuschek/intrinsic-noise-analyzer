#include "ssscanwizard.hh"
#include "../application.hh"
#include "../models/scopeitemmodel.hh"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QCheckBox>


using namespace iNA;




SSScanWizard::SSScanWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("System Size Expansion");

  this->setPage(SSScanWizard::MODEL_SELECTION_PAGE, new SSScanModelSelectionPage(this));
  this->setPage(SSScanWizard::SPECIES_SELECTION_PAGE, new SSScanSpeciesSelectionPage(this));
  this->setPage(SSScanWizard::SCAN_CONFIG_PAGE, new SSScanSpectrumConfigPage(this));
  this->setPage(SSScanWizard::SUMMARY_PAGE, new SSScanSummaryPage(this));
  this->page(SSScanWizard::SUMMARY_PAGE)->setFinalPage(true);
}

GeneralTaskConfig &
SSScanWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation of model selection page:
 * ********************************************************************************************* */
SSScanModelSelectionPage::SSScanModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Select a model to analyze."));
}


bool
SSScanModelSelectionPage::validatePage()
{
  // Try to create LNA model from SBML document
  try {
    // Forward call to parent implementation:
    if (! ModelSelectionWizardPage::validatePage())
      return false;
  } catch (Exception err) {
    // Simply show a warning and done.
    QMessageBox::warning(0, tr("Cannot construct LNA anlysis from model: "), err.what());
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of species selection page:
 * ********************************************************************************************* */
SSScanSpeciesSelectionPage::SSScanSpeciesSelectionPage(GeneralTaskWizard *parent)
  : SpeciesSelectionWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Select some species for analysis."));
}



/* ********************************************************************************************* *
 * Implementation of frequency range page:
 * ********************************************************************************************* */
SSScanSpectrumConfigPage::SSScanSpectrumConfigPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Configure steady state solver."));

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

  p_min = new QLineEdit("0.0");
  p_min->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *p_min_val = new QDoubleValidator();
  p_min->setValidator(p_min_val);
  registerField("p_min", p_min);

  p_max = new QLineEdit("1.0");
  p_max->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *p_max_val = new QDoubleValidator();
  p_max->setValidator(p_max_val);
  registerField("p_max", p_max);

  p_num = new QLineEdit("100");
  p_num->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *p_num_val = new QIntValidator(); p_num_val->setBottom(1);
  p_num->setValidator(p_num_val);
  registerField("p_num", p_num);

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Precision"), epsilon);
  layout->addRow(tr("Max. iterations"), n_iter);
  layout->addRow(tr("Max. integration time"), t_max);
  layout->addRow(tr("Parameter"), p_select);
  layout->addRow(tr("Start value"), p_min);
  layout->addRow(tr("End value"), p_max);
  layout->addRow(tr("Steps"), p_num);

  this->setLayout(layout);
}


void
SSScanSpectrumConfigPage::initializePage()
{
  // Get the config:
  SSScanWizard *wizard = static_cast<SSScanWizard *>(this->wizard());
  SSScanTask::Config &config = wizard->getConfigCast<SSScanTask::Config>();

  // Init parameter list:
  p_select->clear();
  p_select->addItems(ScopeItemModel::collectIdentifiers(
                       *(config.getModel()), ScopeItemModel::SELECT_PARAMETERS));
  if (p_select->model()->rowCount() > 0) { p_select->setCurrentIndex(0); }
}


bool
SSScanSpectrumConfigPage::validatePage()
{
  // Get the wizard:
  SSScanWizard *wizard = static_cast<SSScanWizard *>(this->wizard());
  SSScanTask::Config &config = wizard->getConfigCast<SSScanTask::Config>();

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
 * Implementation of summary page:
 * ********************************************************************************************* */
SSScanSummaryPage::SSScanSummaryPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle("Steady State Analysis");
  this->setSubTitle("Summary");

  this->model_name = new QLabel();
  this->model_name->setTextFormat(Qt::LogText);

  this->species = new QLabel();
  this->species->setTextFormat(Qt::LogText);
  this->species->setWordWrap(true);

  //this->spectrum = new QLabel();
  //this->spectrum->setTextFormat(Qt::LogText);

  this->memory = new QLabel();
  this->memory->setTextFormat(Qt::LogText);

  QFormLayout *layout = new QFormLayout();
  layout->addRow("Model:", this->model_name);
  layout->addRow("Selected species:", this->species);
  //layout->addRow("Calculate spectrum:", this->spectrum);
  layout->addRow("Approx. memory used:", this->memory);
  this->setLayout(layout);
}


void
SSScanSummaryPage::initializePage()
{
  SSScanWizard *wizard = static_cast<SSScanWizard *>(this->wizard());
  SSScanTask::Config &config = wizard->getConfigCast<SSScanTask::Config>();

  this->model_name->setText(config.getModelDocument()->getModel().getName().c_str());
  QStringList species(config.getSelectedSpecies());
  this->species->setText(species.join(", "));

  QString mem_str("%1 MB");
  int N = config.getNumSpecies();
  this->memory->setText(mem_str.arg(double(8*(2*N+N*N))/126976.));
}

