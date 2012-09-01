#include "ssatask.hh"

using namespace iNA;


/* ******************************************************************************************** *
 * Implementation of SSATaskConfig.
 * ******************************************************************************************** */
SSATaskConfig::SSATaskConfig()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), SpeciesSelectionTaskConfig(),
    EngineTaskConfig(), method(SSATaskConfig::DIRECT_SSA), ensemble_size(0), final_time(0),
    steps(0), num_threads(0), simulator(0)
{
  // Pass...
}

SSATaskConfig::SSATaskConfig(const SSATaskConfig &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), SpeciesSelectionTaskConfig(other),
    EngineTaskConfig(other), method(other.method), ensemble_size(other.ensemble_size),
    final_time(other.final_time), steps(other.steps), num_threads(other.num_threads),
    simulator(other.simulator)
{
  // Pass...
}


Models::StochasticSimulator *
SSATaskConfig::getSimulator() const
{
  return simulator;
}


void
SSATaskConfig::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);
  // create copy of model:
  model = new iNA::Ast::Model(getModelDocument()->getModel());
}


Ast::Model *
SSATaskConfig::getModel() const
{
  return model;
}


void
SSATaskConfig::setSimulator(iNA::Models::StochasticSimulator *sim)
{
  simulator=sim;
}


SSATaskConfig::SSAMethod
SSATaskConfig::getMethod() const
{
  return method;
}

void
SSATaskConfig::setMethod(SSAMethod meth)
{
  method = meth;
}


size_t
SSATaskConfig::getEnsembleSize() const
{
  return ensemble_size;
}

void
SSATaskConfig::setEnsembleSize(size_t size)
{
  ensemble_size = size;
}


size_t
SSATaskConfig::getSteps() const
{
  return steps;
}

void
SSATaskConfig::setSteps(size_t steps)
{
  this->steps = steps;
}


double
SSATaskConfig::getFinalTime() const
{
  return final_time;
}

void
SSATaskConfig::setFinalTime(double ftime)
{
  final_time = ftime;
}


size_t
SSATaskConfig::getNumThreads() const
{
  return num_threads;
}

void
SSATaskConfig::setNumThreads(size_t num)
{
  num_threads = num;
}



/* ******************************************************************************************** *
 * Implementation of SSATaskConfig.
 * ******************************************************************************************** */
SSATask::SSATask(const SSATaskConfig &config, QObject *parent)
  : Task(parent), simulator(config.getSimulator()),
    species_id(config.getNumSpecies()), species_name(config.getNumSpecies()),
    final_time(config.getFinalTime()),
    time_series(1+2*config.getNumSpecies()+(config.getNumSpecies()*(config.getNumSpecies()+1))/2,
      config.getSteps()+1),
    mean_index_table(config.getNumSpecies()),
    cov_index_table(config.getNumSpecies(), config.getNumSpecies()),
    skew_index_table(config.getNumSpecies()),
    species_index_table(config.getNumSpecies())
{
  // First, column is time:
  this->time_series.setColumnName(0, "t"); size_t column = 1;

  // Assemble list of species and index_table
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
  {
    // Store species ID
    this->species_id[i] = config.getSelectedSpecies().at(i);

    // Get name of species or use ID as name:
    if (simulator->getSpecies(this->species_id[i].toStdString())->hasName()) {
      this->species_name[i] = simulator->getSpecies(this->species_id[i].toStdString())->getName().c_str();
    } else {
      this->species_name[i] = this->species_id[i];
    }

    // Get index of species:
    this->mean_index_table[i] = column;
    // Associate i-th selected species with original index:
    this->species_index_table[i] = simulator->getSpeciesIdx(this->species_id[i].toStdString());

    // Assign column name for species:
    this->time_series.setColumnName(column, this->species_name[i]);
  }

  // Assign names to covariances:
  for (int i=0; i<this->species_id.size(); i++)
  {
    for (int j=i; j<this->species_id.size(); j++, column++)
    {
      QString name_a = this->species_name[i];
      QString name_b = this->species_name[j];
      this->time_series.setColumnName(column, QString("cov(%1,%2)").arg(name_a).arg(name_b));
      this->cov_index_table(i,j) = column;
      this->cov_index_table(j,i) = column;
    }
  }

  // Assign names to skewness:
  for (int i=0; i<this->species_id.size(); i++, column++) {
    this->time_series.setColumnName(column, QString("Skewness %1").arg(this->species_name[i]));
    this->skew_index_table[i] = column;
  }
}


void
SSATask::process()
{
  this->setState(Task::INITIALIZED);
  double dt = this->final_time/(this->time_series.getNumRows()-1);

  Eigen::VectorXd mean(this->simulator->numSpecies());
  Eigen::MatrixXd cov(this->simulator->numSpecies(), this->simulator->numSpecies());
  Eigen::VectorXd skewness(this->simulator->numSpecies());

  this->setState(Task::RUNNING);
  this->setProgress(0.0);

  for (size_t i=0; i<this->time_series.getNumRows(); i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState())
    {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    // Set current simulation statistics:
    this->simulator->stats(mean, cov, skewness);

    // Assemble timeseries row:
    Eigen::VectorXd row(this->time_series.getNumColumns());
    row(0) = i*dt;
    for (size_t j=0; j<this->numSpecies(); j++) {
      size_t index_j = this->species_index_table[j];
      row(this->mean_index_table[j]) = mean(index_j);
      row(this->skew_index_table[j]) = skewness(index_j);
      for (size_t k=j; k<this->numSpecies(); k++) {
        size_t index_k = this->species_index_table[k];
        row(this->cov_index_table(j,k)) = cov(index_j, index_k);
      }
    }
    this->time_series.append(row);

    // Perfrom simulation:
    this->simulator->run(dt);

    // Update progress bar:
    this->setProgress(double(i+1)/this->time_series.getNumRows());
  }

  // Done:
  this->setProgress(1.0);
  this->setState(Task::DONE);
}


QString
SSATask::getLabel()
{
  return "Stochastic Simulation Algorithm";
}


iNA::Models::StochasticSimulator *
SSATask::getModel()
{
  return this->simulator;
}


Table &
SSATask::getTimeSeries()
{
  return this->time_series;
}


size_t
SSATask::numSpecies() const
{
  return this->species_id.size();
}
