#include "lnasteadystatetask.hh"

/* ******************************************************************************************* *
 * Implementation of LNASteadyStateTask::Config, the task configuration.
 * ******************************************************************************************* */
LNASteadyStateTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), SpeciesSelectionTaskConfig(),
    model(0), max_iterations(0), max_time_step(0), epsilon(0), auto_frequencies(false),
    min_frequency(0), max_frequency(0), num_frequency(0)
{
  // Pass...
}

LNASteadyStateTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), SpeciesSelectionTaskConfig(other),
    model(other.model), max_iterations(other.max_iterations), max_time_step(other.max_time_step),
    epsilon(other.epsilon), auto_frequencies(other.auto_frequencies),
    min_frequency(other.min_frequency), max_frequency(other.max_frequency),
    num_frequency(other.num_frequency)
{
  // Pass...
}

void
LNASteadyStateTask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);
  // Construct LNA model from SBML model associated with the selected document
  this->model = new Fluc::Models::IOSmodel(document->getSBMLModel());
}

Fluc::Ast::Model *
LNASteadyStateTask::Config::getModel() const
{
  return this->model;
}


size_t
LNASteadyStateTask::Config::getMaxIterations() const
{
  return this->max_iterations;
}

void
LNASteadyStateTask::Config::setMaxIterations(size_t num)
{
  this->max_iterations = num;
}

double
LNASteadyStateTask::Config::getMaxTimeStep() const
{
  return max_time_step;
}

void
LNASteadyStateTask::Config::setMaxTimeStep(double t_max)
{
  max_time_step = t_max;
}

double
LNASteadyStateTask::Config::getEpsilon() const
{
  return this->epsilon;
}

void
LNASteadyStateTask::Config::setEpsilon(double eps)
{
  this->epsilon = eps;
}



/* ******************************************************************************************* *
 * Implementation of LNASteadyStateTask::Config, the task configuration.
 * ******************************************************************************************* */
LNASteadyStateTask::LNASteadyStateTask(const Config &config, QObject *parent)
  : Task(parent), config(config),
    steady_state(dynamic_cast<Fluc::Models::IOSmodel &>(*config.getModel()),
      config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep()),
    concentrations(config.getNumSpecies()), emre_corrections(config.getNumSpecies()),
    ios_corrections(config.getNumSpecies()),
    lna_covariances(config.getNumSpecies(), config.getNumSpecies()),
    ios_covariances(config.getNumSpecies(), config.getNumSpecies()),
    species(config.getNumSpecies()), species_name(config.getNumSpecies()),
    spectrum(1, config.getNumSpecies()+1),
    index_table(config.getNumSpecies())
{
  //this->spectrum.setColumnName(0, "freq");

  // Assign species identifier:
  for (int i=0; i<this->species.size(); i++)
  {
    this->species[i] = config.getSelectedSpecies().at(i);
    this->index_table[i] = config.getModel()->getSpeciesIdx(this->species[i].toStdString());

    if (config.getModel()->getSpecies(this->species[i].toStdString())->hasName()) {
      this->species_name[i] = config.getModel()->getSpecies(this->species[i].toStdString())->getName().c_str();
    } else {
      this->species_name[i] = this->species[i];
    }

    //this->spectrum.setColumnName(i+1, this->species_name[i]);
  }

  // Create frequency vector:
  /*double df = (max_freq-min_freq)/num_freq;
  for (size_t i=0; i<num_freq; i++)
  {
    this->frequencies(i) = min_freq+i*df;
  }*/
}



void
LNASteadyStateTask::process()
{
  this->setState(Task::INITIALIZED);
  this->setProgress(0);

  Fluc::Models::IOSmodel *lna_model
      = dynamic_cast<Fluc::Models::IOSmodel *>(config.getModel());

  // Allocate reduced state vector (independent species)
  Eigen::VectorXd reduced_state(lna_model->getDimension());

  // Calc steadystate:
  this->steady_state.calcSteadyState(reduced_state);

  // Check if task shall terminate:
  if (Task::TERMINATING == this->getState())
  {
    this->setState(Task::ERROR, tr("Task was terminated by user."));
    return;
  }

  // Get full state and covariance and EMRE corrections for steady state;
  Eigen::VectorXd concentrations(config.getModel()->numSpecies());
  Eigen::VectorXd emre_corrections(config.getModel()->numSpecies());
  Eigen::VectorXd iosemre_corrections(config.getModel()->numSpecies());
  Eigen::MatrixXd lna_covariances(config.getModel()->numSpecies(), config.getModel()->numSpecies());
  Eigen::MatrixXd ios_covariances(config.getModel()->numSpecies(), config.getModel()->numSpecies());
  Eigen::VectorXd thirdOrder(config.getModel()->numSpecies());
  lna_model->fullState(reduced_state, concentrations, lna_covariances, emre_corrections,
                       ios_covariances, thirdOrder, iosemre_corrections);

//  // Get steadystate spectrum:
//  Eigen::MatrixXd spectrum(this->frequencies.size(), this->model->numSpecies());
//  this->setProgress(0.5);

//  std::cerr << "Calc spectrum ... " << std::endl;
//  this->steady_state.calcSpectrum(this->frequencies, spectrum, auto_frequencies);
//  std::cerr << "   ... done." << std::endl;

  // Copy concentration, EMRE, COVARIANCE
  for (int i=0; i<this->species.size(); i++)
  {
    this->concentrations(i) = concentrations(index_table[i]);
    this->emre_corrections(i) = emre_corrections(index_table[i]);
    this->ios_corrections(i) = iosemre_corrections(index_table[i]);

    for (int j=0; j<this->species.size(); j++)
    {
      this->lna_covariances(i, j) = lna_covariances(index_table[i], index_table[j]);
      this->ios_covariances(i, j) = ios_covariances(index_table[i], index_table[j]);
    }
  }

  // Copy spectrum into table
  /*for (int i=0; i<this->frequencies.size(); i++)
  {
    this->spectrum.matrix()(i,0) = this->frequencies(i);

    for (int j=0; j<this->species.size(); j++)
    {
      size_t idx = this->index_table[j];
      this->spectrum.matrix()(i,j+1) = spectrum(i, idx);
    }
  }*/

  // Done...
  this->setState(Task::DONE);

  {
   Fluc::Utils::Message message = LOG_MESSAGE(Fluc::Utils::Message::INFO);
   message << "Finished steady state analysis.";
   Fluc::Utils::Logger::get().log(message);
  }

}


QString
LNASteadyStateTask::getLabel()
{
  return "Steady State Analysis (SSE)";
}


const Fluc::Ast::Unit &
LNASteadyStateTask::getSpeciesUnit() const
{
  return config.getModel()->getSpecies(0)->getUnit();
}

Eigen::VectorXd &
LNASteadyStateTask::getConcentrations()
{
  return this->concentrations;
}


Eigen::VectorXd &
LNASteadyStateTask::getEMRECorrections()
{
  return this->emre_corrections;
}

Eigen::VectorXd &
LNASteadyStateTask::getIOSCorrections()
{
  return this->ios_corrections;
}

Eigen::MatrixXd &
LNASteadyStateTask::getLNACovariances()
{
  return this->lna_covariances;
}

Eigen::MatrixXd &
LNASteadyStateTask::getIOSCovariances()
{
  return this->ios_covariances;
}

Table &
LNASteadyStateTask::getSpectrum()
{
  return this->spectrum;
}

const QString &
LNASteadyStateTask::getSpeciesId(int i)
{
  return this->species[i];
}

const QString &
LNASteadyStateTask::getSpeciesName(int i)
{
  return this->species_name[i];
}
