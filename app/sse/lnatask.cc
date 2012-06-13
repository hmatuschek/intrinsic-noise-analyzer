#include "lnatask.hh"
#include "ode/ode.hh"


/* ******************************************************************************************** *
 * Implementation of LNATask
 * ******************************************************************************************** */
LNATask::LNATask(const SSETaskConfig &config, QObject *parent) :
  Task(parent), config(config),
  interpreter(*config.getModelAs<Fluc::Models::LNAmodel>(), config.getOptLevel(), config.getNumThreads(), false),
  timeseries(1 + 2*config.getNumSpecies() + config.getNumSpecies()*(config.getNumSpecies()+1)/2,
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps()))
{
  size_t column = 0;

  QVector<QString> species_names(config.getNumSpecies());

  this->timeseries.setColumnName(column, "t"); column++;
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
  {
    QString species_id = config.getSelectedSpecies().value(i);
    Fluc::Ast::Species *species = config.getModel()->getSpecies(species_id.toStdString());

    if (species->hasName())
      species_names[i] = QString("%1").arg(species->getName().c_str());
    else
      species_names[i] = species_id;

    this->timeseries.setColumnName(column, QString("%1").arg(species_names[i]));
  }

  for (int i=0; i<(int)config.getNumSpecies(); i++)
  {
    for (int j=i; j<(int)config.getNumSpecies(); j++, column++)
    {
      this->timeseries.setColumnName(
            column,QString("cov(%1,%2)").arg(species_names[i]).arg(species_names[j]));
    }
  }

  for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
  {
    this->timeseries.setColumnName(
          column, QString("EMRE(%1)").arg(species_names[i]));
  }

  /*
   * Create integrator:
   */
  switch (config.getIntegrator()) {
  case SSETaskConfig::RungeKutta4:
    this->stepper = new Fluc::ODE::RungeKutta4<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize());
    break;

  case SSETaskConfig::RungeKuttaFehlberg45:
    this->stepper = new Fluc::ODE::RKF45<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case SSETaskConfig::DormandPrince5:
    this->stepper = new Fluc::ODE::Dopri853Stepper<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case SSETaskConfig::LSODA:
    this->stepper = new Fluc::ODE::LsodaDriver<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case SSETaskConfig::Rosenbrock4:
    // First, let LNA Interpreter compile the jacobian
    this->interpreter.compileJacobian();
    // Then, setup integrator:
    this->stepper = new Fluc::ODE::Rosenbrock4TimeInd<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;
  }
}



LNATask::~LNATask()
{
  // Free integrator:
  if (0 != this->stepper)
    delete stepper;
}


void
LNATask::process()
{
  // Holds the current system state (reduced state)
  Eigen::VectorXd x(config.getModelAs<Fluc::Models::LNAmodel>()->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(config.getModelAs<Fluc::Models::LNAmodel>()->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(config.getModel()->numSpecies());

  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd cov(config.getModel()->numSpecies(), config.getModel()->numSpecies());

  // Holds EMRE correction for state:
  Eigen::VectorXd emre(config.getModel()->numSpecies());

  // Allocate output vector
  Eigen::VectorXd output_vector(this->timeseries.getNumColumns());

  // Maps the i-th selected species to an index in the concentrations vector:
  size_t N_sel_species = this->config.getNumSpecies();
  std::vector<size_t> species_index(N_sel_species);
  for (size_t i=0; i<N_sel_species; i++)
  {
    species_index[i] = this->config.getModel()->getSpeciesIdx(
          this->config.getSelectedSpecies().value(i).toStdString());
  }

  // initialize (reduced) state
  config.getModelAs<Fluc::Models::LNAmodel>()->getInitialState(x);
  // get full initial concentrations and covariance
  config.getModelAs<Fluc::Models::LNAmodel>()->fullState(x, concentrations, cov, emre);

  this->setState(Task::RUNNING);

  /*
   * Perform integration
   */
  double t  = config.getIntegrationRange().getStartTime();
  double dt = config.getIntegrationRange().getStepSize();

  // store initial state:
  output_vector(0) = t;
  for (size_t j=0; j<N_sel_species; j++)
  {
    size_t index_i = species_index[j];
    output_vector(1+j) = concentrations(index_i);
  }
  size_t idx = 1 + N_sel_species;
  for (size_t j=0; j<N_sel_species; j++)
  {
    size_t index_i = species_index[j];
    for (size_t k=j; k<N_sel_species; k++, idx++)
    {
      size_t index_j = species_index[k];
      output_vector(idx) = cov(index_i, index_j);
    }
  }
  for (size_t j=0; j<N_sel_species; j++, idx++)
  {
    size_t index_i = species_index[j];
    output_vector(idx) = emre(index_i) + concentrations(index_i);
  }
  this->timeseries.append(output_vector);

  // Integration loop:
  for (size_t i=0; i<config.getIntegrationRange().getSteps(); i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState())
    {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(i)/config.getIntegrationRange().getSteps());

    // Determine update:
    this->stepper->step(x, t, dx);
    // Update state:
    x+=dx;
    // Update time
    t += dt;

    // Skip immediate steps
    if(0 != config.getIntermediateSteps() && 0 != i%(1+config.getIntermediateSteps()))
      continue;

    // Get full state:
    config.getModelAs<Fluc::Models::LNAmodel>()->fullState(x, concentrations, cov, emre);

    // Store new time:
    output_vector(0) = t;

    // Store states of selected species:
    for (size_t j=0; j<N_sel_species; j++)
    {
      size_t index_i = species_index[j];
      output_vector(1+j) = concentrations(index_i);
    }

    // Store cov() of species.
    size_t idx = 1 + N_sel_species;
    for (size_t j=0; j<N_sel_species; j++)
    {
      size_t index_i = species_index[j];
      for (size_t k=j; k<N_sel_species; k++, idx++)
      {
        size_t index_j = species_index[k];
        output_vector(idx) = cov(index_i, index_j);
      }
    }

    // Store EMRE correction + LNA state:
    for (size_t j=0; j<N_sel_species; j++, idx++)
    {
      size_t index_i = species_index[j];
      output_vector(idx) = emre(index_i) + concentrations(index_i);
    }

    this->timeseries.append(output_vector);
  }

  // Finally send last progress event:
  this->setProgress(1.0);
  this->setState(Task::DONE);
}


QString
LNATask::getLabel()
{
  return "Time Course Analysis (SSE)";
}


Table *
LNATask::getTimeSeries()
{
  return &(this->timeseries);
}


const QList<QString> &
LNATask::getSelectedSpecies() const
{
  return config.getSelectedSpecies();
}


const Fluc::Ast::Unit &
LNATask::getSpeciesUnit() const
{
  return this->config.getModelAs<Fluc::Models::LNAmodel>()->getConcentrationUnit();
}


const Fluc::Ast::Unit &
LNATask::getTimeUnit() const
{
  return this->config.getModelAs<Fluc::Models::LNAmodel>()->getTimeUnit();
}
