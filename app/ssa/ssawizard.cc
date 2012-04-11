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



SSAWizard::SSAWizard(QWidget *parent) :
    QWizard(parent)
{
  this->setWindowTitle("Stochastic Simulation Algorihtm");

  this->setPage(SSAWizard::MODEL_SELECTION_PAGE, new SSAModelSelectionPage());
  this->setPage(SSAWizard::SPECIES_SELECTION_PAGE, new SSASpeciesSelectionPage());
  this->setPage(SSAWizard::CONFIG_PAGE, new SSAConfigPage());
  this->setPage(SSAWizard::SUMMARY_PAGE, new SSASummaryPage());

  this->page(SSAWizard::SUMMARY_PAGE)->setFinalPage(true);
  this->setStartId(SSAWizard::MODEL_SELECTION_PAGE);
}


void
SSAWizard::setDocument(DocumentItem *document)
{
  this->document = document;
}


DocumentItem *
SSAWizard::getDocument()
{
  return this->document;
}


void
SSAWizard::setSelectedSpecies(const QList<QString> &species)
{
  this->selected_species = species;
}


const QList<QString> &
SSAWizard::getSelectedSpecies()
{
  return this->selected_species;
}


void
SSAWizard::setEnsembleSize(size_t size)
{
  this->ensemble_size = size;
}


size_t
SSAWizard::getEnsembleSize()
{
  return this->ensemble_size;
}


void
SSAWizard::setFinalTime(double time)
{
  this->final_time = time;
}


double
SSAWizard::getFinalTime()
{
  return this->final_time;
}


void
SSAWizard::setNumSamples(size_t size)
{
  this->num_samples = size;
}


size_t
SSAWizard::getNumSamples()
{
  return this->num_samples;
}


void
SSAWizard::setMethod(const QString &method)
{
  this->method = method;
}


const QString &
SSAWizard::getMethod()
{
  return this->method;
}


void
SSAWizard::setOptimizeByteCode(bool optimize)
{
  this->optimize_bytecode = optimize;
}


bool
SSAWizard::getOptimizeByteCode()
{
  return this->optimize_bytecode;
}


void
SSAWizard::setThreadCount(size_t count)
{
  this->thread_count = count;
}


size_t
SSAWizard::getThreadCount()
{
  return this->thread_count;
}


void
SSAWizard::setSimulator(Fluc::Models::StochasticSimulator *simulator)
{
  this->simulator = simulator;
}


Fluc::Models::StochasticSimulator *
SSAWizard::getSimulator()
{
  return this->simulator;
}


SSATaskConfig
SSAWizard::getConfig()
{
  return SSATaskConfig(this->simulator, this->selected_species,
                       this->final_time, this->num_samples);
}




/* ********************************************************************************************* *
 * Implementation model selection page:
 * ********************************************************************************************* */
SSAModelSelectionPage::SSAModelSelectionPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Stochastic Simulation Algorithm"));
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


bool
SSAModelSelectionPage::validatePage()
{
  // Get the wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());

  // Get selected item:
  TreeItem *item = static_cast<TreeItem *>(
        Application::getApp()->docTree()->index(
          this->modelSelection->currentIndex(),0).internalPointer());

  // Set document to wizard:
  wizard->setDocument(static_cast<DocumentItem *>(item));

  return true;
}



/* ********************************************************************************************* *
 * Implementation of the species selection page:
 * ********************************************************************************************* */
SSASpeciesSelectionPage::SSASpeciesSelectionPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Stochastic Simulation Algorithm"));
  this->setSubTitle(tr("Select some species for analysis."));

  this->speciesList = new QListWidget();
  this->registerField("Species*", this->speciesList);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->speciesList);
  this->setLayout(layout);
}


void
SSASpeciesSelectionPage::initializePage()
{
  // First cleanup list of species:
  while(this->speciesList->count())
  {
    delete this->speciesList->takeItem(0);
  }

  // Get list of species from selected model:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());
  Fluc::Ast::Model *model = wizard->getDocument()->getModel();
  for (size_t i=0; i<model->numSpecies(); i++)
  {
    QString species_name;
    QString species_id;

    species_id = model->getSpecies(i)->getIdentifier().c_str();
    if (model->getSpecies(i)->hasName())
    {
      species_name = model->getSpecies(i)->getName().c_str();
    }
    else
    {
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
SSASpeciesSelectionPage::validatePage()
{
  // Just collect the species to plot:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());

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
  wizard->setSelectedSpecies(list_of_species);

  // Done.
  return true;
}



/* ********************************************************************************************* *
 * Implementation SSA config page:
 * ********************************************************************************************* */
SSAConfigPage::SSAConfigPage(QWidget *parent)
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

  this->method = new QComboBox();
  this->method->addItem(tr("Direct SSA"), QVariant("direct"));
  this->method->addItem(tr("Optimized SSA"), QVariant("optimized"));
  this->registerField("method", this->method);

  QCheckBox *bytecode_opt = new QCheckBox();
  bytecode_opt->setChecked(true);
  this->registerField("optimize_bytecode", bytecode_opt);

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
  layout->addRow(tr("Optimize byte-code"), bytecode_opt);
  layout->addRow(tr("Thread count"), thread_count);

  this->setLayout(layout);
}


bool
SSAConfigPage::validatePage()
{
  // Get the wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());

  QString method = this->method->itemData(this->field("method").toUInt()).toString();

  bool valid = false;
  wizard->setEnsembleSize(this->field("size").toUInt(&valid));
  wizard->setFinalTime(this->field("time").toDouble(&valid));
  wizard->setNumSamples(this->field("steps").toUInt(&valid));
  wizard->setMethod(method);
  wizard->setOptimizeByteCode(this->field("optimize_bytecode").toBool());
  wizard->setThreadCount(this->field("thread_count").toInt(&valid));

  return valid;
}




/* ********************************************************************************************* *
 * Implementation SSA summary page
 * ********************************************************************************************* */
SSASummaryPage::SSASummaryPage(QWidget *parent)
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

  this->optimize_bytecode = new QLabel("");
  this->optimize_bytecode->setTextFormat(Qt::LogText);

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
  layout->addRow(tr("Optimize bytecode"), this->optimize_bytecode);
  layout->addRow(tr("Memory usage"), this->mem_usage);
  layout->addRow(tr("Thread count"), this->thread_count);
  this->setLayout(layout);
}


void
SSASummaryPage::initializePage()
{
  // Get the wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());

  QStringList species(wizard->getSelectedSpecies());
  this->selected_species->setText(species.join(", "));

  this->document->setText(QString("%1").arg(wizard->getDocument()->getSBMLModel()->getName().c_str()));
  this->ensemble_size->setText(QString("%1").arg(wizard->getEnsembleSize()));
  this->final_time->setText(QString("%1").arg(wizard->getFinalTime()));
  this->num_samples->setText(QString("%1").arg(wizard->getNumSamples()));
  this->method->setText(QString("%1").arg(wizard->getMethod()));
  this->optimize_bytecode->setText(QString("%1").arg(wizard->getOptimizeByteCode()));
  this->thread_count->setText(QString("%1").arg(wizard->getThreadCount()));
  int mem_usage = 8*wizard->getNumSamples()*(1 + species.size() + (species.size()*(1+species.size()))/2);
  this->mem_usage->setText(QString("%1MB").arg(double(mem_usage)/(1024*1024)));
}


bool
SSASummaryPage::validatePage()
{
  // Get the configured wizard:
  SSAWizard *wizard = static_cast<SSAWizard *>(this->wizard());

  // Construct simulation algorithm:
  Fluc::Models::StochasticSimulator *simulator=0;

  try {
    if (wizard->getMethod() == "direct") {
      simulator = new Fluc::Models::GillespieSSA(wizard->getDocument()->getSBMLModel(),
                                                 wizard->getEnsembleSize(), time(0),
                                                 wizard->getOptimizeByteCode(),
                                                 wizard->getThreadCount());
    } else if (wizard->getMethod() == "optimized"){
      simulator = new Fluc::Models::OptimizedSSA(wizard->getDocument()->getSBMLModel(),
                                                 wizard->getEnsembleSize(), time(0),
                                                 wizard->getOptimizeByteCode(),
                                                 wizard->getThreadCount());
    } else {
      QMessageBox::warning(
            0, tr("Can not create SSA analysis:"),
            tr("Unknown method: %0").arg(wizard->getMethod()));
      return false;
    }
  }
  catch (Fluc::Exception &err)
  {
    QMessageBox::warning(0, tr("Can not construct SSA anlysis: "), err.what());
    return false;
  }

  wizard->setSimulator(simulator);

  return true;
}
