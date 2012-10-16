#include "ssawizard.hh"

#include "openmp.hh"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QMessageBox>
#include <QSpinBox>


#include "../application.hh"

using namespace iNA;



SSAWizard::SSAWizard(QWidget *parent) :
  GeneralTaskWizard(parent), config()
{
  this->setWindowTitle("Stochastic Simulation Algorithm");

  this->setPage(SSAWizard::MODEL_SELECTION_PAGE, new SSAModelSelectionPage(this));
  //this->setPage(SSAWizard::SPECIES_SELECTION_PAGE, new SSASpeciesSelectionPage(this));
  this->setPage(SSAWizard::ENGINE_SELECTION_PAGE, new SSAEngineSelectionPage(this));
  this->setPage(SSAWizard::CONFIG_PAGE, new SSAConfigPage(this));
  this->setPage(SSAWizard::SUMMARY_PAGE, new SSASummaryPage(this));

  this->page(SSAWizard::SUMMARY_PAGE)->setFinalPage(true);
  this->setStartId(SSAWizard::MODEL_SELECTION_PAGE);
}


GeneralTaskConfig &
SSAWizard::getConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation model selection page:
 * ********************************************************************************************* */
SSAModelSelectionPage::SSAModelSelectionPage(GeneralTaskWizard *parent)
  : ModelSelectionWizardPage(parent)
{
  this->setTitle(tr("Stochastic Simulation Algorithm"));
  this->setSubTitle(tr("Select a model to analyze."));
}


bool SSAModelSelectionPage::validatePage() {
  if (! ModelSelectionWizardPage::validatePage()) { return false; }
  SSAWizard *_wizard = static_cast<SSAWizard *>(wizard());
  iNA::Ast::Model *model = _wizard->getConfigCast<SSATaskConfig>().getModel();

  // Remove that as soon as possible:
  QStringList selected_species;
  for (size_t i=0; i<model->numSpecies(); i++) {
    selected_species.append(model->getSpecies(i)->getIdentifier().c_str());
  }
  _wizard->getConfigCast<SSATaskConfig>().setSelectedSpecies(selected_species);
  return true;
}


/* ********************************************************************************************* *
 * Implementation of the species selection page:
 * ********************************************************************************************* */
SSASpeciesSelectionPage::SSASpeciesSelectionPage(GeneralTaskWizard *parent)
  : SpeciesSelectionWizardPage(parent)
{
  this->setTitle(tr("Stochastic Simulation Algorithm"));
  this->setSubTitle(tr("Select some species for analysis."));
}



/* ********************************************************************************************* *
 * Implementation of the engine selection page:
 * ********************************************************************************************* */
SSAEngineSelectionPage::SSAEngineSelectionPage(GeneralTaskWizard *parent)
  : EngineWizardPage(parent, false)
{
  this->setTitle(tr("Stochastic Simulation Algorithm"));
  setSubTitle(tr("Select engine for propensity evaluation"));
}


/* ********************************************************************************************* *
 * Implementation SSA config page:
 * ********************************************************************************************* */
SSAConfigPage::SSAConfigPage(SSAWizard *parent)
  : QWizardPage(parent)
{
  QLineEdit *size = new QLineEdit("30");
  QIntValidator *size_val = new QIntValidator(0);
  size_val->setBottom(1); size->setValidator(size_val);
  this->registerField("size", size);

  QLineEdit *time = new QLineEdit("1.0");
  QDoubleValidator *time_val = new QDoubleValidator(0);
  time_val->setBottom(0.0); time->setValidator(time_val);
  this->registerField("time", time);

  QLineEdit *steps = new QLineEdit("100");
  QIntValidator *steps_val = new QIntValidator(0);
  steps_val->setBottom(2); steps->setValidator(steps_val);
  this->registerField("steps", steps);

  method = new QComboBox();
  method->addItem(tr("Optimized SSA"), SSATaskConfig::OPTIMIZED_SSA);
  method->addItem(tr("Direct SSA"), SSATaskConfig::DIRECT_SSA);
  method->addItem(tr("Next-Reaction SSA"), SSATaskConfig::GIBSONBRUCK_SSA);
  method->setCurrentIndex(0);

  QSpinBox *thread_count = new QSpinBox();
  thread_count->setMinimum(1);
  thread_count->setMaximum(OpenMP::getMaxThreads());
  thread_count->setValue(OpenMP::getMaxThreads());
  this->registerField("thread_count", thread_count);
  if (1 == OpenMP::getMaxThreads())
    thread_count->setEnabled(false);

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Ensemble size"), size);
  layout->addRow(tr("Final time"), time);
  layout->addRow(tr("Plot points"), steps);
  layout->addRow(tr("SSA method"), method);
  layout->addRow(tr("Thread count"), thread_count);

  size->setToolTip("Number of stochastic sample paths used for statistical average.");
  time->setToolTip("Final time of simulation.");
  steps->setToolTip("Specifies the number of individual time points from which statistical average is obtained.");
  method->setToolTip("You can use the optimized exact SSA method for all purposes.");
  thread_count->setToolTip("iNA can take advantage of multiple CPUs to simulate multiple sample paths in parallel.");

  this->setLayout(layout);
}


bool
SSAConfigPage::validatePage()
{
  // Get the wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());
  SSATaskConfig &config = wizard->getConfigCast<SSATaskConfig>();

  SSATaskConfig::SSAMethod method = SSATaskConfig::SSAMethod(
        this->method->itemData(this->method->currentIndex()).toUInt());
  config.setMethod(method);

  bool valid = false;
  config.setEnsembleSize(this->field("size").toUInt(&valid));
  config.setFinalTime(this->field("time").toDouble(&valid));
  config.setSteps(this->field("steps").toUInt(&valid));
  config.setNumThreads(this->field("thread_count").toInt(&valid));

  return valid;
}




/* ********************************************************************************************* *
 * Implementation SSA summary page
 * ********************************************************************************************* */
SSASummaryPage::SSASummaryPage(SSAWizard *parent)
  : QWizardPage(parent)
{
  this->document = new QLabel("");
  this->document->setTextFormat(Qt::LogText);

  this->selected_species = new QLabel("");
  this->selected_species->setTextFormat(Qt::LogText);
  this->selected_species->setWordWrap(true);

  this->ensemble_size = new QLabel("");
  this->ensemble_size->setTextFormat(Qt::LogText);

  this->final_time = new QLabel("");
  this->final_time->setTextFormat(Qt::LogText);

  this->num_samples = new QLabel("");
  this->num_samples->setTextFormat(Qt::LogText);

  this->method = new QLabel("");
  this->method->setTextFormat(Qt::LogText);

  this->mem_usage = new QLabel("");
  this->mem_usage->setTextFormat(Qt::LogText);

  this->thread_count = new QLabel("");
  this->thread_count->setTextFormat(Qt::LogText);

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Model"), this->document);
  layout->addRow(tr("Selected species"), this->selected_species);
  layout->addRow(tr("Ensemble size"), this->ensemble_size);
  layout->addRow(tr("Final time"), this->final_time);
  layout->addRow(tr("Plot points"), this->num_samples);
  layout->addRow(tr("Method"), this->method);
  layout->addRow(tr("Memory usage"), this->mem_usage);
  layout->addRow(tr("Thread count"), this->thread_count);
  this->setLayout(layout);
}


void
SSASummaryPage::initializePage()
{
  // Get the wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());
  SSATaskConfig &config = wizard->getConfigCast<SSATaskConfig>();

  QStringList species(config.getSelectedSpecies());
  this->selected_species->setText(species.join(", "));

  this->document->setText(QString("%1").arg(config.getModelDocument()->getModel().getName().c_str()));
  this->ensemble_size->setText(QString("%1").arg(config.getEnsembleSize()));
  this->final_time->setText(QString("%1").arg(config.getFinalTime()));
  this->num_samples->setText(QString("%1").arg(config.getSteps()));
  switch (config.getMethod()) {
  case SSATaskConfig::DIRECT_SSA:
    this->method->setText("Direct SSA");
    break;

  case SSATaskConfig::OPTIMIZED_SSA:
    this->method->setText("Optimized SSA");
    break;

  case SSATaskConfig::GIBSONBRUCK_SSA:
    this->method->setText("Next-Reaction SSA");
    break;
  }
  this->thread_count->setText(QString("%1").arg(config.getNumThreads()));
  int mem_usage = 8*config.getSteps()*(1 + species.size() + (species.size()*(1+species.size()))/2);
  this->mem_usage->setText(QString("%1MB").arg(double(mem_usage)/(1024*1024)));
}


bool
SSASummaryPage::validatePage()
{
  // Get the configured wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());
  SSATaskConfig &config = wizard->getConfigCast<SSATaskConfig>();

  // Construct simulation algorithm:
  Models::StochasticSimulator *simulator=0;

  try {
    // Switch simulator generation by engine type:
    switch (config.getEngine()) {

    case EngineTaskConfig::GINAC_ENGINE:
      switch (config.getMethod()) {
      case SSATaskConfig::DIRECT_SSA:
        simulator = new Models::GenericGillespieSSA< Eval::direct::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      case SSATaskConfig::OPTIMIZED_SSA:
        simulator = new Models::GenericOptimizedSSA< Eval::direct::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

    case SSATaskConfig::GIBSONBRUCK_SSA:
      simulator = new Models::GenericGibsonBruckSSA< Eval::direct::Engine<Eigen::VectorXd> >(
            config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
            config.getOptLevel(), config.getNumEvalThreads());
      break;
    } break;


    case EngineTaskConfig::BCI_ENGINE:
    case EngineTaskConfig::BCIMP_ENGINE:
      switch (config.getMethod()) {
      case SSATaskConfig::DIRECT_SSA:
        simulator = new Models::GenericGillespieSSA< Eval::bci::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      case SSATaskConfig::OPTIMIZED_SSA:
        simulator = new Models::GenericOptimizedSSA< Eval::bci::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      case SSATaskConfig::GIBSONBRUCK_SSA:
        simulator = new Models::GenericGibsonBruckSSA< Eval::bci::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      } break;

    case EngineTaskConfig::JIT_ENGINE:
      switch (config.getMethod()) {
      case SSATaskConfig::DIRECT_SSA:
        simulator = new Models::GenericGillespieSSA< Eval::jit::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      case SSATaskConfig::OPTIMIZED_SSA:
        simulator = new Models::GenericOptimizedSSA< Eval::jit::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      case SSATaskConfig::GIBSONBRUCK_SSA:
        simulator = new Models::GenericGibsonBruckSSA< Eval::jit::Engine<Eigen::VectorXd> >(
              config.getModelDocument()->getModel(), config.getEnsembleSize(), time(0),
              config.getOptLevel(), config.getNumEvalThreads());
        break;

      } break;
    }
  }
  catch (iNA::Exception &err)
  {
    QMessageBox::warning(0, tr("Can not construct SSA analysis: "), err.what());
    return false;
  }

  config.setSimulator(simulator);

  return true;
}
