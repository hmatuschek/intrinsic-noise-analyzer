#include "lnawizard.hh"
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
 * Implementation of the LNA configuration wizard:
 * ********************************************************************************************* */
LNAWizard::LNAWizard(QWidget *parent) :
  QWizard(parent), document(0), config()
{
  this->setWindowTitle("System Size Expansion");

  this->setPage(LNAWizard::MODEL_SELECTION_PAGE, new LNAModelSelectionPage());
  this->setPage(LNAWizard::SPECIES_SELECTION_PAGE, new LNASpeciesSelectionPage());
  this->setPage(LNAWizard::INTEGRATOR_CONFIG_PAGE, new LNAIntegratorConfigPage());
  this->setPage(LNAWizard::SUMMARY_PAGE, new LNASummaryPage());
  this->page(LNAWizard::SUMMARY_PAGE)->setFinalPage(true);

  // Set some default values for the task-config:
  config.numThreads = OpenMP::getMaxThreads();
  config.optLevel   = 1;
}


void
LNAWizard::setDocument(DocumentItem *doc)
{
  this->document = doc;
  this->config.model = new Models::LinearNoiseApproximation(doc->getSBMLModel());
}


DocumentItem *
LNAWizard::getDocument() const
{
  return this->document;
}


LNATask::Config &
LNAWizard::getTaskConfig()
{
  return this->config;
}



/* ********************************************************************************************* *
 * Implementation model selection page, checks and constructs the LNA analysis model
 * from SBML model on exit:
 * ********************************************************************************************* */
LNAModelSelectionPage::LNAModelSelectionPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Time Course analysis"));
  this->setSubTitle(tr("Select a model to analyze."));

  QVBoxLayout *layout = new QVBoxLayout();
  this->modelSelection = new QComboBox();
  this->modelSelection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  this->modelSelection->setFixedWidth(400);
  this->modelSelection->setModel(Application::getApp()->docTree());
  this->registerField("Model*", this->modelSelection);

  layout->addWidget(this->modelSelection);
  this->setLayout(layout);
}


void
LNAModelSelectionPage::initializePage()
{
  if (0 < this->modelSelection->model()->rowCount()) {
    this->modelSelection->setCurrentIndex(0);
  }
}


bool
LNAModelSelectionPage::validatePage()
{
  // Get the wizard:
  LNAWizard *wizard = static_cast<LNAWizard *>(this->wizard());

  try {
    DocumentTree *docs = Application::getApp()->docTree();
    TreeItem *item = static_cast<TreeItem *>(
          docs->index(this->modelSelection->currentIndex(),0).internalPointer());
    wizard->setDocument(static_cast<DocumentItem *>(item));
  } catch (Exception err) {
    // Simply show a warning and done.
    QMessageBox::warning(0, tr("Can not construct LNA anlysis from model: "), err.what());
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of the species selection page:
 * ********************************************************************************************* */
LNASpeciesSelectionPage::LNASpeciesSelectionPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Time Course Analysis"));
  this->setSubTitle(tr("Select some species for analysis."));

  this->speciesList = new QListWidget();
  this->registerField("Species*", this->speciesList);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->speciesList);
  this->setLayout(layout);
}


void
LNASpeciesSelectionPage::initializePage()
{
  // First cleanup list of species:
  while(this->speciesList->count())
  {
    delete this->speciesList->takeItem(0);
  }

  // Get list of species from selected model:
  LNAWizard *wizard = static_cast<LNAWizard *>(this->wizard());
  for (size_t i=0; i<wizard->getTaskConfig().model->numSpecies(); i++)
  {
    QString species_name;
    QString species_id;

    if (wizard->getTaskConfig().model->getSpecies(i)->hasName())
    {
      species_id   = wizard->getTaskConfig().model->getSpecies(i)->getIdentifier().c_str();
      species_name = wizard->getTaskConfig().model->getSpecies(i)->getName().c_str();
    }
    else
    {
      species_id   = wizard->getTaskConfig().model->getSpecies(i)->getIdentifier().c_str();
      species_name = species_id;
    }

    QListWidgetItem *item = new QListWidgetItem(species_name);
    item->setData(Qt::UserRole, species_id);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    this->speciesList->addItem(item);
  }
}


bool
LNASpeciesSelectionPage::validatePage()
{
  // Just collect the species to plot:
  LNAWizard *wizard = static_cast<LNAWizard *>(this->wizard());

  QList<QString> list_of_species;
  for (int i=0; i<this->speciesList->count(); i++)
  {
    if (Qt::Checked == this->speciesList->item(i)->checkState())
    {
      list_of_species.append(this->speciesList->item(i)->data(Qt::UserRole).toString());
    }
  }

  // There should be at least one species selected
  if (0 == list_of_species.size())
  {
    return false;
  }

  // Set list of selected species:
  wizard->getTaskConfig().selected_species = list_of_species;

  // Done.
  return true;
}




/* ********************************************************************************************* *
 * Implementation of the integrator configuration page
 * ********************************************************************************************* */
LNAIntegratorConfigPage::LNAIntegratorConfigPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle("Time Course Analysis");
  this->setSubTitle("Set parameters");

  QLineEdit *t = new QLineEdit("1.0");
  t->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *t_val = new QDoubleValidator(0);
  t_val->setBottom(0.0); t->setValidator(t_val);
  this->registerField("t", t);

  QLineEdit *n = new QLineEdit();
  n->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *n_val = new QIntValidator(0);
  n_val->setBottom(2); n->setValidator(n_val); n->setText("100");
  this->registerField("n", n);

  intermediateSteps = new QLineEdit("0");
  this->intermediateSteps->setEnabled(false);
  intermediateSteps->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QIntValidator *intermediateSteps_val = new QIntValidator();
  intermediateSteps_val->setBottom(0); intermediateSteps->setValidator(intermediateSteps_val);
  this->registerField("n_imm", intermediateSteps);

  this->integrator = new QComboBox();
  this->integrator->addItem("RKF45 (adaptive)", QVariant("rkf45"));
  this->integrator->addItem("Dopri5 (adaptive)", QVariant("dopr5"));
  this->integrator->addItem("Rosenbrock4 (stiff)", QVariant("ros4"));
  this->integrator->addItem("RK4", QVariant("rk4"));
  this->integrator->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->registerField("integrator", this->integrator);
  QObject::connect(this->integrator, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(onIntegratorSelected(int)));

  this->ep_abs = new QLineEdit("1e-10");
  ep_abs->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *ep_abs_val = new QDoubleValidator(0);
  ep_abs_val->setBottom(0); ep_abs->setValidator(ep_abs_val);
  this->registerField("epsilon_abs", ep_abs);

  this->ep_rel = new QLineEdit("1e-4");
  ep_rel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  QDoubleValidator *ep_rel_val = new QDoubleValidator(0);
  ep_abs_val->setBottom(0); ep_rel->setValidator(ep_rel_val);
  this->registerField("epsilon_rel", ep_rel);

  this->optByteCode = new QCheckBox();
  this->optByteCode->setChecked(true);
  this->optByteCode->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->registerField("opt_bytecode", this->optByteCode);

  this->numThreads = new QSpinBox();
  this->numThreads->setMinimum(1);
  this->numThreads->setMaximum(OpenMP::getMaxThreads());
  this->numThreads->setValue(OpenMP::getMaxThreads());
  if (1 == OpenMP::getMaxThreads())
    this->numThreads->setEnabled(false);
  this->numThreads->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  this->registerField("num_threads", this->numThreads);

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Final time"), t);
  layout->addRow(tr("Number of time steps"), n);
  layout->addRow(tr("Number of intermediate steps"), intermediateSteps);
  layout->addRow(tr("Integrator"), this->integrator);
  layout->addRow(tr("Max. absolute error"), ep_abs);
  layout->addRow(tr("Max. relative error"), ep_rel);
  layout->addRow(tr("Optimize byte-code"), this->optByteCode);
  layout->addRow(tr("Thread count"), this->numThreads);
  this->setLayout(layout);
}


void
LNAIntegratorConfigPage::onIntegratorSelected(int index)
{
  QVariant value = this->integrator->itemData(index);
  if ("rk4" == value.toString())
  {
    this->ep_abs->setEnabled(false);
    this->ep_rel->setEnabled(false);
    this->intermediateSteps->setEnabled(true);
  }
  else
  {
    this->ep_abs->setEnabled(true);
    this->ep_rel->setEnabled(true);
    this->intermediateSteps->setEnabled(false);
  }
}


bool
LNAIntegratorConfigPage::validatePage()
{
  double t0 = 0.0;
  double t  = this->field("t").toDouble();
  int n     = this->field("n").toInt();
  int n_imm = this->field("n_imm").toInt();
  double epsilon_abs = this->field("epsilon_abs").toDouble();
  double epsilon_rel = this->field("epsilon_rel").toDouble();

  if (t<=t0 || n<2 || n_imm < 0 || epsilon_abs==0 || epsilon_rel==0)
  {
    return false;
  }

  LNAWizard *wizard = static_cast<LNAWizard *>(this->wizard());

  wizard->getTaskConfig().integration_range = Fluc::ODE::IntegrationRange(t0, t, n*(1+n_imm));
  wizard->getTaskConfig().epsilon_abs = epsilon_abs;
  wizard->getTaskConfig().epsilon_rel = epsilon_rel;
  wizard->getTaskConfig().immediate_steps = n_imm;

  if (this->optByteCode->isChecked()) {
    wizard->getTaskConfig().optLevel = 1;
  } else {
    wizard->getTaskConfig().optLevel = 0;
  }

  wizard->getTaskConfig().numThreads = this->numThreads->value();

  if ("rk4" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    wizard->getTaskConfig().integrator = LNATask::Config::RungeKutta4;
  } else if ("rkf45" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    wizard->getTaskConfig().integrator = LNATask::Config::RungeKuttaFehlberg45;
  } else if ("dopr5" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    wizard->getTaskConfig().integrator = LNATask::Config::DormandPrince5;
  } else if ("ros4" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    wizard->getTaskConfig().integrator = LNATask::Config::Rosenbrock4;
  } else {
    return false;
  }

  return true;
}



/* ********************************************************************************************* *
 * Implementation of the summary page
 * ********************************************************************************************* */
LNASummaryPage::LNASummaryPage(QWidget *parent)
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
LNASummaryPage::initializePage()
{
  LNAWizard *wizard = static_cast<LNAWizard *>(this->wizard());
  this->model_name->setText(wizard->getDocument()->getSBMLModel()->getName().c_str());

  this->selected_species->setText(wizard->getTaskConfig().selected_species.join(", "));

  ODE::IntegrationRange range = wizard->getTaskConfig().integration_range;
  QString range_str("From %1 to %2 in %3 steps.");
  this->integration_range->setText(
        range_str.arg(range.getStartTime()).arg(range.getEndTime()).arg(range.getSteps()));

  size_t N_species = wizard->getTaskConfig().selected_species.size();
  size_t N = 1 + 2*N_species + (N_species*(N_species+1))/2;
  QString mem_str("%1 MB");
  this->memory->setText(mem_str.arg(double(8*N*range.getSteps())/126976));
}
