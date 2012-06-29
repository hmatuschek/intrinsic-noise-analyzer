#include "ssewizard.hh"
#include "../application.hh"
#include "exception.hh"

#include <QMessageBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>


using namespace Fluc;




/* ********************************************************************************************* *
 * Implementation of the IOS configuration wizard:
 * ********************************************************************************************* */
SSEWizard::SSEWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("Time Course Analysis (SSE)");

  this->setPage(SSEWizard::MODEL_SELECTION_PAGE, new SSEModelSelectionPage(this));
  this->setPage(SSEWizard::SPECIES_SELECTION_PAGE, new SSESpeciesSelectionPage(this));
  this->setPage(SSEWizard::ENGINE_SELECTION_PAGE, new SSEEngineSelectionPage(this));
  this->setPage(SSEWizard::INTEGRATOR_CONFIG_PAGE, new SSEIntegratorPage(this));
  this->setPage(SSEWizard::SUMMARY_PAGE, new SSESummaryPage(this));
  this->page(SSEWizard::SUMMARY_PAGE)->setFinalPage(true);
}


GeneralTaskConfig &
SSEWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation model selection page, checks and constructs the LNA analysis model
 * from SBML model on exit:
 * ********************************************************************************************* */
SSEModelSelectionPage::SSEModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Time Course analysis (SSE)"));
  this->setSubTitle(tr("Select a model to analyze and the method."));

  // Append method selection radio buttons to the wizard page.
  _re_button  = new QRadioButton("Rate Equations (REs)");
  _lna_button = new QRadioButton("Linear Noise Approximation (LNA)");
  _ios_button = new QRadioButton("Inverse Omega Square (IOS)");

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
SSEModelSelectionPage::validatePage()
{
  // First set the analysis method, this deterines which model will be instantiated:
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  SSETaskConfig &config = wizard->getConfigCast<SSETaskConfig>();
  if (_re_button->isChecked())
    config.setMethod(SSETaskConfig::RE_ANALYSIS);
  else if (_lna_button->isChecked())
    config.setMethod(SSETaskConfig::LNA_ANALYSIS);
  else
    config.setMethod(SSETaskConfig::IOS_ANALYSIS);

  // Now, call default handler, this will instantiate the selected analyis model,
  // if there is any exception, show error message.
  try {
    if (! ModelSelectionWizardPage::validatePage()) {
      return false;
    }
  } catch (Exception err) {
    // Simply show a warning and done.
    QMessageBox::warning(0, tr("Can not construct RE anlysis from model: "), err.what());
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of the species selection page:
 * ********************************************************************************************* */
SSESpeciesSelectionPage::SSESpeciesSelectionPage(GeneralTaskWizard *parent)
  : SpeciesSelectionWizardPage(parent)
{
  this->setTitle(tr("Time Course Analysis (SSE)"));
  this->setSubTitle(tr("Select some species for analysis."));
}



/* ********************************************************************************************* *
 * Implementation of the integrator configuration page
 * ********************************************************************************************* */
SSEEngineSelectionPage::SSEEngineSelectionPage(GeneralTaskWizard *parent)
  : EngineWizardPage(parent, true)
{
  setTitle("Time Course Analysis (SSE)");
  setSubTitle("Execution engine");
}



/* ********************************************************************************************* *
 * Implementation of the integrator configuration page
 * ********************************************************************************************* */
SSEIntegratorPage::SSEIntegratorPage(GeneralTaskWizard *parent)
  : IntegratorWizardPage(parent)
{
  this->setTitle("Time Course Analysis (SSE)");
  this->setSubTitle("Set parameters");
}



/* ********************************************************************************************* *
 * Implementation of the summary page
 * ********************************************************************************************* */
SSESummaryPage::SSESummaryPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle("Time Course Analysis (SSE)");
  this->setSubTitle("Summary");

  this->model_name = new QLabel();
  this->model_name->setTextFormat(Qt::LogText);

  this->selected_species = new QLabel();
  this->selected_species->setTextFormat(Qt::LogText);
  this->selected_species->setWordWrap(true);

  this->integration_range = new QLabel();
  this->integration_range->setTextFormat(Qt::LogText);

  this->memory = new QLabel();
  this->memory->setTextFormat(Qt::LogText);

  QFormLayout *layout = new QFormLayout();
  layout->addRow("Model:", this->model_name);
  layout->addRow("Selected species:", this->selected_species);
  layout->addRow("Plot range:", this->integration_range);
  layout->addRow("Approx. memory used:", this->memory);
  this->setLayout(layout);
}


void
SSESummaryPage::initializePage()
{
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  SSETaskConfig &config = wizard->getConfigCast<SSETaskConfig>();

  this->model_name->setText(config.getModelDocument()->getSBMLModel()->getName().c_str());
  this->selected_species->setText(config.getSelectedSpecies().join(", "));

  ODE::IntegrationRange range = config.getIntegrationRange();
  QString range_str("From %1 to %2 in %3 steps.");
  this->integration_range->setText(
        range_str.arg(range.getStartTime()).arg(range.getEndTime()).arg(range.getSteps()));

  size_t N_species = config.getNumSpecies();
  size_t N = 1 + 2*N_species + (N_species*(N_species+1));
  QString mem_str("%1 MB");
  this->memory->setText(mem_str.arg(double(8*N*range.getSteps())/126976));
}
