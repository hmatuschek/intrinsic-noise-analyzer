#include "rewizard.hh"
#include "../application.hh"
#include "exception.hh"

#include <QMessageBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>



using namespace Fluc;




/* ********************************************************************************************* *
 * Implementation of the LNA configuration wizard:
 * ********************************************************************************************* */
REWizard::REWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("Time Course Analysis");

  this->setPage(REWizard::MODEL_SELECTION_PAGE, new REModelSelectionPage(this));
  this->setPage(REWizard::SPECIES_SELECTION_PAGE, new RESpeciesSelectionPage(this));
  this->setPage(REWizard::INTEGRATOR_CONFIG_PAGE, new REIntegratorPage(this));
  this->setPage(REWizard::SUMMARY_PAGE, new RESummaryPage(this));
  this->page(REWizard::SUMMARY_PAGE)->setFinalPage(true);
}


GeneralTaskConfig &
REWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation model selection page, checks and constructs the LNA analysis model
 * from SBML model on exit:
 * ********************************************************************************************* */
REModelSelectionPage::REModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Time Course analysis"));
  this->setSubTitle(tr("Select a model to analyze."));
}


bool
REModelSelectionPage::validatePage()
{
  // Call default handler, if there is any exception, show error message.
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
RESpeciesSelectionPage::RESpeciesSelectionPage(GeneralTaskWizard *parent)
  : SpeciesSelectionWizardPage(parent)
{
  this->setTitle(tr("Time Course Analysis"));
  this->setSubTitle(tr("Select some species for analysis."));
}



/* ********************************************************************************************* *
 * Implementation of the integrator configuration page
 * ********************************************************************************************* */
REIntegratorPage::REIntegratorPage(GeneralTaskWizard *parent)
  : IntegratorWizardPage(parent)
{
  this->setTitle("Time Course Analysis");
  this->setSubTitle("Set parameters");
}



/* ********************************************************************************************* *
 * Implementation of the summary page
 * ********************************************************************************************* */
RESummaryPage::RESummaryPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle("Time Course Analysis");
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
RESummaryPage::initializePage()
{
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  RETask::Config &config = wizard->getConfigCast<RETask::Config>();

  this->model_name->setText(config.getModelDocument()->getSBMLModel()->getName().c_str());
  this->selected_species->setText(config.getSelectedSpecies().join(", "));

  ODE::IntegrationRange range = config.getIntegrationRange();
  QString range_str("From %1 to %2 in %3 steps.");
  this->integration_range->setText(
        range_str.arg(range.getStartTime()).arg(range.getEndTime()).arg(range.getSteps()));

  size_t N_species = config.getNumSpecies();
  size_t N = 1 + 2*N_species + (N_species*(N_species+1))/2;
  QString mem_str("%1 MB");
  this->memory->setText(mem_str.arg(double(8*N*range.getSteps())/126976));
}
