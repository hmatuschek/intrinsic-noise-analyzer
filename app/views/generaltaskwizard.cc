#include "generaltaskwizard.hh"
#include "../application.hh"
#include "../models/generaltaskconfig.hh"
#include "openmp.hh"

#include <QVBoxLayout>
#include <QFormLayout>


/* ******************************************************************************************* *
 * Implementation of GeneralTaskWizard
 * ******************************************************************************************* */
GeneralTaskWizard::GeneralTaskWizard(QWidget *parent)
  : QWizard(parent)
{
  // Pass...
}



/* ******************************************************************************************* *
 * Implementation of ModelSelectionWizardPage
 * ******************************************************************************************* */
ModelSelectionWizardPage::ModelSelectionWizardPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
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
ModelSelectionWizardPage::initializePage()
{
  if (0 < this->modelSelection->model()->rowCount()) {
    this->modelSelection->setCurrentIndex(0);
  }
}

bool
ModelSelectionWizardPage::validatePage()
{
  // Get the wizard:
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());

  DocumentTree *docs = Application::getApp()->docTree();
  TreeItem *item = static_cast<TreeItem *>(
        docs->index(this->modelSelection->currentIndex(),0).internalPointer());
  DocumentItem *doc = dynamic_cast<DocumentItem *>(item);
  wizard->getConfigCast<ModelSelectionTaskConfig>().setModelDocument(doc);

  return true;
}



/* ******************************************************************************************* *
 * Implementation of ModelSelectionWizardPage
 * ******************************************************************************************* */
SpeciesSelectionWizardPage::SpeciesSelectionWizardPage(GeneralTaskWizard *parent)
  : QWizardPage(parent)
{
  this->speciesList = new QListWidget();
  this->registerField("Species*", this->speciesList);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->speciesList);
  this->setLayout(layout);
}


void
SpeciesSelectionWizardPage::initializePage()
{
  // First cleanup list of species:
  while(this->speciesList->count()) {
    delete this->speciesList->takeItem(0);
  }

  // Get selected model from wizard:
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  SpeciesSelectionTaskConfig &config = wizard->getConfigCast<SpeciesSelectionTaskConfig>();
  Fluc::Ast::Model *model = config.getModel();

  // Construct list of species
  for (size_t i=0; i<model->numSpecies(); i++)
  {
    QString species_name;
    QString species_id;

    if (model->getSpecies(i)->hasName())
    {
      species_id   = model->getSpecies(i)->getIdentifier().c_str();
      species_name = model->getSpecies(i)->getName().c_str();
    }
    else
    {
      species_id   = model->getSpecies(i)->getIdentifier().c_str();
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
SpeciesSelectionWizardPage::validatePage()
{
  // Just collect the species to plot:
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());

  QList<QString> list_of_species;
  for (int i=0; i<this->speciesList->count(); i++)
  {
    if (Qt::Checked == this->speciesList->item(i)->checkState()) {
      list_of_species.append(this->speciesList->item(i)->data(Qt::UserRole).toString());
    }
  }

  // There should be at least one species selected
  if (0 == list_of_species.size()) {
    return false;
  }

  // Set list of selected species:
  wizard->getConfigCast<SpeciesSelectionTaskConfig>().setSelectedSpecies(list_of_species);

  // Done.
  return true;
}



/* ********************************************************************************************* *
 * Implementation of the integrator configuration page
 * ********************************************************************************************* */
IntegratorWizardPage::IntegratorWizardPage(QWidget *parent)
  : QWizardPage(parent)
{
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
  this->integrator->addItem("LSODA", QVariant("lsoda"));
  this->integrator->addItem("RKF45 (adaptive)", QVariant("rkf45"));
  this->integrator->addItem("Dopri5 (adaptive)", QVariant("dopr5"));
  this->integrator->addItem("Rosenbrock4 (stiff)", QVariant("ros4"));
//  this->integrator->addItem("RK4", QVariant("rk4"));
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
IntegratorWizardPage::onIntegratorSelected(int index)
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
IntegratorWizardPage::validatePage()
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

  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  ODEIntTaskConfig &config = wizard->getConfigCast<ODEIntTaskConfig>();

  config.setIntegrationRange(Fluc::ODE::IntegrationRange(t0, t, n*(1+n_imm)));
  config.setEpsilon(epsilon_abs, epsilon_rel);
  config.setIntermediateSteps(n_imm);

  if (this->optByteCode->isChecked()) {
    config.setOptLevel(1);
  } else {
    config.setOptLevel(0);
  }

  config.setNumThreads(this->numThreads->value());

  if ("rk4" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    config.setIntegrator(ODEIntTaskConfig::RungeKutta4);
  } else if ("rkf45" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    config.setIntegrator(ODEIntTaskConfig::RungeKuttaFehlberg45);
  } else if ("dopr5" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    config.setIntegrator(ODEIntTaskConfig::DormandPrince5);
  }  else if ("lsoda" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
        config.setIntegrator(ODEIntTaskConfig::LSODA);
  } else if ("ros4" == this->integrator->itemData(this->integrator->currentIndex()).toString()) {
    config.setIntegrator(ODEIntTaskConfig::Rosenbrock4);
  } else {
    return false;
  }

  return true;
}

