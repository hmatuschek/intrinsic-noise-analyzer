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
  QWizard(parent), frequency_min(0), frequency_max(1), num_frequencies(0)
{
  this->setWindowTitle("System Size Expansion");

  this->setPage(LNASteadyStateWizard::MODEL_SELECTION_PAGE, new LNASteadyStateModelSelectionPage());
  this->setPage(LNASteadyStateWizard::SPECIES_SELECTION_PAGE, new LNASteadyStateSpeciesSelectionPage());
  this->setPage(LNASteadyStateWizard::FREQUENCY_RANGE_PAGE, new LNASteadyStateSpectrumConfigPage());
  this->setPage(LNASteadyStateWizard::SUMMARY_PAGE, new LNASteadyStateSummaryPage());
  this->page(LNASteadyStateWizard::SUMMARY_PAGE)->setFinalPage(true);
}


Models::LinearNoiseApproximation *
LNASteadyStateWizard::getLNAModel()
{
  return this->lna_model;
}


void
LNASteadyStateWizard::setDocument(DocumentItem *doc)
{
  this->document = doc;
  this->lna_model = new Models::LinearNoiseApproximation(doc->getSBMLModel());
}


DocumentItem *
LNASteadyStateWizard::getDocument() const
{
  return this->document;
}


void
LNASteadyStateWizard::setSelectedSpecies(const QList<QString> &species)
{
  this->selected_species = species;
}

const QList<QString> &
LNASteadyStateWizard::getSelectedSpecies()
{
  return this->selected_species;
}


void
LNASteadyStateWizard::setFrequencyAutoRange(bool automatic)
{
  this->frequency_auto_range = automatic;
}


void
LNASteadyStateWizard::setFrequencyRange(double min, double max, size_t num)
{
  this->frequency_min = min;
  this->frequency_max = max;
  this->num_frequencies = num;
}


bool
LNASteadyStateWizard::getAutomaticFrequencies()
{
  return this->frequency_auto_range;
}


size_t
LNASteadyStateWizard::getNumFrequencies()
{
  return this->num_frequencies;
}


double
LNASteadyStateWizard::getMinFrequency()
{
  return this->frequency_min;
}


double
LNASteadyStateWizard::getMaxFrequency()
{
  return this->frequency_max;
}


void
LNASteadyStateWizard::setNumIterations(size_t num)
{
  this->num_iterations = num;
}

size_t
LNASteadyStateWizard::getNumIterations()
{
  return this->num_iterations;
}


void
LNASteadyStateWizard::setEpsilon(double eps)
{
  this->epsilon = eps;
}


double
LNASteadyStateWizard::getEpsilon()
{
  return this->epsilon;
}



/* ********************************************************************************************* *
 * Implementation of model selection page:
 * ********************************************************************************************* */
LNASteadyStateModelSelectionPage::LNASteadyStateModelSelectionPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
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
LNASteadyStateModelSelectionPage::validatePage()
{
  // Get the wizard:
  LNASteadyStateWizard *wizard = static_cast<LNASteadyStateWizard *>(this->wizard());

  // Try to create LNA model from SBML document
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
 * Implementation of species selection page:
 * ********************************************************************************************* */
LNASteadyStateSpeciesSelectionPage::LNASteadyStateSpeciesSelectionPage(QWidget *parent)
  : QWizardPage(parent)
{
  this->setTitle(tr("Steady State Analysis"));
  this->setSubTitle(tr("Select some species for analysis."));

  this->speciesList = new QListWidget();
  this->registerField("Species*", this->speciesList);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(this->speciesList);
  this->setLayout(layout);
}


void
LNASteadyStateSpeciesSelectionPage::initializePage()
{
  // First cleanup list of species:
  while(this->speciesList->count())
  {
    delete this->speciesList->takeItem(0);
  }

  // Get list of species from selected model:
  LNASteadyStateWizard *wizard = static_cast<LNASteadyStateWizard *>(this->wizard());
  for (size_t i=0; i<wizard->getLNAModel()->numSpecies(); i++)
  {
    QString species_name;
    QString species_id;

    if (wizard->getLNAModel()->getSpecies(i)->hasName())
    {
      species_id   = wizard->getLNAModel()->getSpecies(i)->getIdentifier().c_str();
      species_name = wizard->getLNAModel()->getSpecies(i)->getName().c_str();
    }
    else
    {
      species_id   = wizard->getLNAModel()->getSpecies(i)->getIdentifier().c_str();
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
LNASteadyStateSpeciesSelectionPage::validatePage()
{
  // Just collect the species to plot:
  LNASteadyStateWizard *wizard = static_cast<LNASteadyStateWizard *>(this->wizard());

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
 * Implementation of frequency range page:
 * ********************************************************************************************* */
LNASteadyStateSpectrumConfigPage::LNASteadyStateSpectrumConfigPage(QWidget *parent)
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

//  wizard->setFrequencyAutoRange(this->field("f_automatic").toBool());
//  wizard->setFrequencyRange(this->field("f_min").toDouble(),
//                            this->field("f_max").toDouble(),
//                            this->field("f_num").toUInt());
  wizard->setNumIterations(this->field("n_iter").toInt());
  wizard->setEpsilon(this->field("epsilon").toDouble());

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

  this->model_name->setText(wizard->getDocument()->getSBMLModel()->getName().c_str());

  QStringList species(wizard->getSelectedSpecies());
  this->species->setText(species.join(", "));

//  QString spectrum("From %1 to %2 in %3 steps");
//  this->spectrum->setText(
//        spectrum.arg(wizard->getMinFrequency()).arg(wizard->getMaxFrequency()).arg(wizard->getNumFrequencies()));

//  if (wizard->getAutomaticFrequencies())
//    this->spectrum->setText("automatic");

  QString mem_str("%1 MB");
  int N = wizard->getDocument()->getSBMLModel()->getNumSpecies();
  this->memory->setText(mem_str.arg(double(8*(2*N+N*N))/126976.));
}

