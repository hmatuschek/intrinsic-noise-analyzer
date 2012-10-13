#include "lnasteadystatetask.hh"

/* ******************************************************************************************* *
 * Implementation of LNASteadyStateTask::Config, the task configuration.
 * ******************************************************************************************* */
LNASteadyStateTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(),
    model(0), max_iterations(0), max_time_step(0), epsilon(0), auto_frequencies(false),
    min_frequency(0), max_frequency(0), num_frequency(0)
{
  // Pass...
}

LNASteadyStateTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other),
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
  this->model = new iNA::Models::IOSmodel(document->getModel());
}

iNA::Ast::Model *
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
  : Task(parent), config(config), _Ns(config.getModel()->numSpecies()),
    steady_state(dynamic_cast<iNA::Models::IOSmodel &>(*config.getModel()),
      config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep()),
    concentrations(_Ns), emre_corrections(_Ns), ios_corrections(_Ns),
    lna_covariances(_Ns, _Ns), ios_covariances(_Ns, _Ns),
    spectrum(1, _Ns+1)
{
  // Pass...
}



void
LNASteadyStateTask::process()
{
  this->setState(Task::INITIALIZED);
  this->setProgress(0);

  iNA::Models::IOSmodel *lna_model
      = dynamic_cast<iNA::Models::IOSmodel *>(config.getModel());

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
  Eigen::VectorXd thirdOrder(config.getModel()->numSpecies());
  lna_model->fullState(reduced_state, this->concentrations, this->lna_covariances, this->emre_corrections,
                       this->ios_covariances, thirdOrder, this->ios_corrections);

//  // Get steadystate spectrum:
//  Eigen::MatrixXd spectrum(this->frequencies.size(), this->model->numSpecies());
//  this->setProgress(0.5);

//  std::cerr << "Calc spectrum ... " << std::endl;
//  this->steady_state.calcSpectrum(this->frequencies, spectrum, auto_frequencies);
//  std::cerr << "   ... done." << std::endl;

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
   iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
   message << "Finished steady state analysis.";
   iNA::Utils::Logger::get().log(message);
  }

}


QString
LNASteadyStateTask::getLabel()
{
  return "Steady State Analysis (SSE)";
}


iNA::Ast::Unit
LNASteadyStateTask::getSpeciesUnit() const
{
  return config.getModel()->getSpeciesUnit();
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

QString
LNASteadyStateTask::getSpeciesId(int i)
{
  return config.getModel()->getSpecies(i)->getIdentifier().c_str();
}

QString
LNASteadyStateTask::getSpeciesName(int i)
{
  iNA::Ast::Species *species = config.getModel()->getSpecies(i);
  if (species->hasName()) { return species->getName().c_str(); }
  return species->getIdentifier().c_str();
}
