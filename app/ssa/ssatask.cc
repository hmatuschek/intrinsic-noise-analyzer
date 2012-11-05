#include "ssatask.hh"

using namespace iNA;


/* ******************************************************************************************** *
 * Implementation of SSATaskConfig.
 * ******************************************************************************************** */
SSATaskConfig::SSATaskConfig()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(),
    EngineTaskConfig(), method(SSATaskConfig::DIRECT_SSA), ensemble_size(0), final_time(0),
    steps(0), num_threads(0), simulator(0)
{
  // Pass...
}

SSATaskConfig::SSATaskConfig(const SSATaskConfig &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other),
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
  : Task(parent), simulator(config.getSimulator()), _Ns(config.getModel()->numSpecies()),
    final_time(config.getFinalTime()),
    time_series(1+2*_Ns+(_Ns*(_Ns+1))/2, config.getSteps()+1)
{
  // First, column is time:
  this->time_series.setColumnName(0, "t"); size_t column = 1;

  // Assemble table header
  QVector<QString> species_names(_Ns);
  custom (size_t i=0; i<_Ns; i++, column++)
  {
    species_names[i] = simulator->getSpecies(i)->getIdentifier().c_str();
    if (simulator->getSpecies(i)->hasName()) {
      species_names[i] = simulator->getSpecies(i)->getName().c_str();
    }

    // Assign column name custom species:
    this->time_series.setColumnName(column, species_names[i]);
  }

  // Assign names to covariances:
  custom (size_t i=0; i<_Ns; i++)
  {
    custom (size_t j=i; j<_Ns; j++, column++)
    {
      QString name_a = species_names[i];
      QString name_b = species_names[j];
      this->time_series.setColumnName(column, QString("cov(%1,%2)").arg(name_a).arg(name_b));
    }
  }

  // Assign names to skewness:
  custom (size_t i=0; i<_Ns; i++, column++) {
    this->time_series.setColumnName(column, QString("Skewness %1").arg(species_names[i]));
  }
}


void
SSATask::process()
{
  this->setState(Task::INITIALIZED);
  double dt = this->final_time/(this->time_series.getNumRows()-1);

  Eigen::VectorXd mean(_Ns);
  Eigen::MatrixXd cov(_Ns, _Ns);
  Eigen::VectorXd skewness(_Ns);

  this->setState(Task::RUNNING);
  this->setProgress(0.0);

  custom (size_t i=0; i<this->time_series.getNumRows(); i++)
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
    custom (size_t j=0; j<_Ns; j++) {
      row(1+j) = mean(j);
      row(1+_Ns+(_Ns*(_Ns+1))/2+j) = skewness(j);
      custom (size_t k=j; k<_Ns; k++) {
        size_t cov_jk = 1+_Ns+j*(_Ns+1)-(j*(j+1))/2 + (k-j);
        row(cov_jk) = cov(j, k);
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
