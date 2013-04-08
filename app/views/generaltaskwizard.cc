#include "generaltaskwizard.hh"
#include "../doctree/documenttree.hh"
#include "../models/application.hh"
#include "../models/generaltaskconfig.hh"
#include "openmp.hh"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>


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
  _modelSelection = new QComboBox();
  _modelSelection->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  _modelSelection->setFixedWidth(400);
  _modelSelection->setModel(Application::getApp()->docTree());
  //this->registerField("Model*", this->modelSelection);

  layout->addWidget(this->_modelSelection);
  setLayout(layout);
}

void
ModelSelectionWizardPage::initializePage()
{
  if (0 < _modelSelection->model()->rowCount()) {
    _modelSelection->setCurrentIndex(0);
  }
}

bool
ModelSelectionWizardPage::validatePage()
{
  // Check if a model was selected:
  if ((0 > _modelSelection->currentIndex()) ||
      (_modelSelection->currentIndex() >= _modelSelection->model()->rowCount())) {
    return false;
  }

  // Get the wizard:
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());
  // Get the selected document (model)
  DocumentTree *docs = Application::getApp()->docTree();
  TreeItem *item = static_cast<TreeItem *>(
        docs->index(_modelSelection->currentIndex(),0).internalPointer());
  DocumentItem *doc = dynamic_cast<DocumentItem *>(item);
  // Store selected model into config.
  wizard->getConfigCast<ModelSelectionTaskConfig>().setModelDocument(doc);

  return true;
}



/* ******************************************************************************************* *
 * Implementation of SpeciesSelectionWizardPage
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
  iNA::Ast::Model *model = config.getModel();

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
    //item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
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
 * Implementation of the engine configuration page
 * ********************************************************************************************* */
EngineWizardPage::EngineWizardPage(GeneralTaskWizard *parent, bool show_parallel_engine)
  : QWizardPage(parent), _show_parallel_engine(show_parallel_engine)
{
  // Construct engine selection.
  engineList = new QComboBox();
  engineList->addItem("JIT compiler", uint(EngineTaskConfig::JIT_ENGINE));
  engineList->addItem("Bytecode interpreter", uint(EngineTaskConfig::BCIMP_ENGINE));

  engineList->setItemData(0, "This option is very fast. \n"
                             "The just-in-time compiler (JIT) automatically compiles the mathematical model representation into executable code.", Qt::ToolTipRole);
  engineList->setItemData(1, "This option is moderately fast. \n"
                             "The bytecode interpreter executes compact numeric codes of the mathematical model representation.", Qt::ToolTipRole);

  engineList->setCurrentIndex(0);

  // Construct code opt
  codeOpt = new QCheckBox();
  codeOpt->setToolTip("Code optimization can be advantageous for computational expensive calculations.");
  codeOpt->setChecked(false);

  // Construct thread count
  numThreads = new QSpinBox(this);
  numThreads->setMinimum(1);
  numThreads->setMaximum(OpenMP::getMaxThreads());
  numThreads->setValue(OpenMP::getMaxThreads());
  numThreads->setEnabled(false);  // Disable num-threads field since default engine is BCI
  numThreads->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  numThreads->setToolTip("The bytecode interpreter can make use of multiple CPUs depending on your hardware.");
  if (! _show_parallel_engine)
    numThreads->setVisible(false);


  // Connect signals:
  QObject::connect(engineList, SIGNAL(currentIndexChanged(int)), this, SLOT(engineSelected(int)));

  // Assemble layout
  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Execution engine"), engineList);
  layout->addRow(tr("Optimize code"), codeOpt);
  if (_show_parallel_engine)
    layout->addRow(tr("Number of threads"), numThreads);
  this->setLayout(layout);
}


bool
EngineWizardPage::validatePage()
{
  // Just get wizard
  GeneralTaskWizard *wizard = static_cast<GeneralTaskWizard *>(this->wizard());

  // Set engine kind.
  EngineTaskConfig::EngineKind kind = EngineTaskConfig::EngineKind(
        engineList->itemData(engineList->currentIndex()).toUInt());
  wizard->getConfigCast<EngineTaskConfig>().setEngine(kind);

  // Set code opt level
  if (codeOpt->isChecked())
    wizard->getConfigCast<EngineTaskConfig>().setOptLevel(1);
  else
    wizard->getConfigCast<EngineTaskConfig>().setOptLevel(0);

  // Set num threads
  wizard->getConfigCast<EngineTaskConfig>().setNumEvalThreads(numThreads->value());

  return true;
}


void
EngineWizardPage::engineSelected(int index)
{
  if (1 == OpenMP::getMaxThreads())
    numThreads->setEnabled(false);

  EngineTaskConfig::EngineKind kind = EngineTaskConfig::EngineKind(
        engineList->itemData(engineList->currentIndex()).toUInt());

  if (EngineTaskConfig::BCIMP_ENGINE == kind) {
    numThreads->setEnabled(true);
  } else {
    numThreads->setEnabled(false);
  }
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

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Final time"), t);
  layout->addRow(tr("Plot points"), n);
  layout->addRow(tr("Number of intermediate steps"), intermediateSteps);
  layout->addRow(tr("Integrator"), this->integrator);
  layout->addRow(tr("Max. absolute error"), ep_abs);
  layout->addRow(tr("Max. relative error"), ep_rel);

  t->setToolTip("Final time of integration.");
  n->setToolTip("Number of individual time points for output.");
  intermediateSteps->setToolTip("Number of additional steps to be taken. \n"
                                "Increasing this number can improve the accuracy of the computation at the cost of slower runtime.");
  integrator->setToolTip("Sets the numerical algorithm for ODE integration. \n LSODA is a good general-purpose choice.");

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
  else if ("rkf45" == value.toString() || "dopr5" == value.toString())
  {
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

  config.setIntegrationRange(iNA::ODE::IntegrationRange(t0, t, n*(1+n_imm)));
  config.setEpsilon(epsilon_abs, epsilon_rel);
  config.setIntermediateSteps(n_imm);

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

