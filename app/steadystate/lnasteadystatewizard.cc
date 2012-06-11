#include "lnasteadystatewizard.hh"
#include "../application.hh"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QCheckBox>


using namespace Fluc;




LNASteadyStateWizard::LNASteadyStateWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("System Size Expansion");

  this->setPage(LNASteadyStateWizard::MODEL_SELECTION_PAGE, new LNASteadyStateModelSelectionPage(this));
  this->setPage(LNASteadyStateWizard::SPECIES_SELECTION_PAGE, new LNASteadyStateSpeciesSelectionPage(this));
  this->setPage(LNASteadyStateWizard::FREQUENCY_RANGE_PAGE, new LNASteadyStateSpectrumConfigPage(this));
  this->setPage(LNASteadyStateWizard::SUMMARY_PAGE, new LNASteadyStateSummaryPage(this));
  this->page(LNASteadyStateWizard::SUMMARY_PAGE)->setFinalPage(true);
}

GeneralTaskConfig &
LNASteadyStateWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation of model selection page:
 * ********************************************************************************************* */
LNASteadyStateModelSelectionPage::LNASteadyStateModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Select a model to analyze."));
}


bool
LNASteadyStateModelSelectionPage::validatePage()
{
  // Try to create LNA model from SBML document
  try {
    // Forward call to parent implementation:
    if (! ModelSelectionWizardPage::validatePage())
      return false;
  } catch (Exception err) {
    // Simply show a warning and done.
    QMessageBox::warning(0, tr("Can not construct LNA anlysis from model: "), err.what());
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of species selection page:
 * ********************************************************************************************* */
LNASteadyStateSpeciesSelectionPage::LNASteadyStateSpeciesSelectionPage(GeneralTaskWizard *parent)
  : SpeciesSelectionWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Select some species for analysis."));
}



/* ********************************************************************************************* *
 * Implementation of frequency range page:
 * ********************************************************************************************* */
LNASteadyStateSpectrumConfigPage::LNASteadyStateSpectrumConfigPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Configure steady state solver."));

  this->n_iter = new QLineEdit(); this->n_iter->setText("1000");
  this->n_iter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *n_iter_val = new QIntValidator(); n_iter_val->setBottom(1);
  this->n_iter->setValidator(n_iter_val);
  this->registerField("n_iter", this->n_iter);

  this->epsilon = new QLineEdit(); this->epsilon->setText("1e-9");
  this->epsilon->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *epsilon_val = new QDoubleValidator(0); epsilon_val->setBottom(1);
  this->epsilon->setValidator(epsilon_val);
  this->registerField("epsilon", this->epsilon);

//  QCheckBox *f_automatic = new QCheckBox();
//  f_automatic->setChecked(true);
//  this->registerField("f_automatic", f_automatic);

//  this->f_min = new QLineEdit("0.0");
//  this->f_min->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
//  QDoubleValidator *f_min_val = new QDoubleValidator(0);
//  f_min_val->setBottom(0.0); this->f_min->setValidator(f_min_val);
//  this->registerField("f_min", this->f_min);
//  this->f_min->setEnabled(false);

//  this->f_max = new QLineEdit("1.0");
//  this->f_max->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
//  QDoubleValidator *f_max_val = new QDoubleValidator(0);
//  f_max_val->setBottom(0.0); this->f_max->setValidator(f_max_val);
//  this->registerField("f_max", this->f_max);
//  this->f_max->setEnabled(false);

//  this->f_num = new QLineEdit("100");
//  this->f_num->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
//  QIntValidator *f_num_val = new QIntValidator(0);
//  f_num_val->setBottom(1); this->f_num->setValidator(f_num_val);
//  this->registerField("f_num", this->f_num);
//  this->f_num->setEnabled(false);

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Max. iterations"), this->n_iter);
  layout->addRow(tr("Precision"), this->epsilon);
  //layout->addRow(tr("Automatic frequency range"), f_automatic);
  //layout->addRow(tr("Minimum frequency"), f_min);
  //layout->addRow(tr("Maximum frequency"), f_max);
  //layout->addRow(tr("Number of plot points"), f_num);

  this->setLayout(layout);

  // Connect events:
  //QObject::connect(f_automatic, SIGNAL(toggled(bool)), this, SLOT(fAutomaticToggled(bool)));
}


bool
LNASteadyStateSpectrumConfigPage::validatePage()
{
  // Get the wizard:
  LNASteadyStateWizard *wizard = static_cast<LNASteadyStateWizard *>(this->wizard());
  LNASteadyStateTask::Config &config = wizard->getConfigCast<LNASteadyStateTask::Config>();

//  wizard->setFrequencyAutoRange(this->field("f_automatic").toBool());
//  wizard->setFrequencyRange(this->field("f_min").toDouble(),
//                            this->field("f_max").toDouble(),
//                            this->field("f_num").toUInt());
  config.setMaxIterations(this->field("n_iter").toInt());
  config.setEpsilon(this->field("epsilon").toDouble());

  return true;
}


void
LNASteadyStateSpectrumConfigPage::fAutomaticToggled(bool value)
{
  this->f_min->setEnabled(! value);
  this->f_max->setEnabled(! value);
  this->f_num->setEnabled(! value);
}



/* ********************************************************************************************* *
 * Implementation of summary page:
 * ********************************************************************************************* */
LNASteadyStateSummaryPage::LNASteadyStateSummaryPage(QWidget *parent)
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
LNASteadyStateSummaryPage::initializePage()
{
  LNASteadyStateWizard *wizard = static_cast<LNASteadyStateWizard *>(this->wizard());
  LNASteadyStateTask::Config &config = wizard->getConfigCast<LNASteadyStateTask::Config>();

  this->model_name->setText(config.getModelDocument()->getSBMLModel()->getName().c_str());
  QStringList species(config.getSelectedSpecies());
  this->species->setText(species.join(", "));

//  QString spectrum("From %1 to %2 in %3 steps");
//  this->spectrum->setText(
//        spectrum.arg(wizard->getMinFrequency()).arg(wizard->getMaxFrequency()).arg(wizard->getNumFrequencies()));

//  if (wizard->getAutomaticFrequencies())
//    this->spectrum->setText("automatic");

  QString mem_str("%1 MB");
  int N = config.getNumSpecies();
  this->memory->setText(mem_str.arg(double(8*(2*N+N*N))/126976.));
}

