#include "ioswizard.hh"
#include "../application.hh"
#include "models/linearnoiseapproximation.hh"
#include "exception.hh"

#include <QMessageBox>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>



using namespace Fluc;




/* ********************************************************************************************* *
 * Implementation of the IOS configuration wizard:
 * ********************************************************************************************* */
IOSWizard::IOSWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("Time Course Analysis (IOS)");

  this->setPage(IOSWizard::MODEL_SELECTION_PAGE, new IOSModelSelectionPage(this));
  this->setPage(IOSWizard::SPECIES_SELECTION_PAGE, new IOSSpeciesSelectionPage(this));
  this->setPage(IOSWizard::INTEGRATOR_CONFIG_PAGE, new IOSIntegratorPage(this));
  this->setPage(IOSWizard::SUMMARY_PAGE, new IOSSummaryPage(this));
  this->page(IOSWizard::SUMMARY_PAGE)->setFinalPage(true);
}


GeneralTaskConfig &
IOSWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation model selection page, checks and constructs the LNA analysis model
 * from SBML model on exit:
 * ********************************************************************************************* */
IOSModelSelectionPage::IOSModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Time Course analysis (IOS)"));
  this->setSubTitle(tr("Select a model to analyze."));
}


bool
IOSModelSelectionPage::validatePage()
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
IOSSpeciesSelectionPage::IOSSpeciesSelectionPage(GeneralTaskWizard *parent)
  : SpeciesSelectionWizardPage(parent)
{
  this->setTitle(tr("Time Course Analysis (IOS)"));
  this->setSubTitle(tr("Select some species for analysis."));
}



/* ********************************************************************************************* *
 * Implementation of the integrator configuration page
 * ********************************************************************************************* */
IOSIntegratorPage::IOSIntegratorPage(GeneralTaskWizard *parent)
  : IntegratorWizardPage(parent)
{
  this->setTitle("Time Course Analysis (IOS)");
  this->setSubTitle("Set parameters");
}



/* ********************************************************************************************* *
 * Implementation of the summary page
 * ********************************************************************************************* */
IOSSummaryPage::IOSSummaryPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->setTitle("Time Course Analysis (IOS)");
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
IOSSummaryPage::initializePage()
{
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  IOSTask::Config &config = wizard->getConfigCast<IOSTask::Config>();

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
