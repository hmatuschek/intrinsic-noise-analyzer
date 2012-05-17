#include "lnatask.hh"
#include "ode/ode.hh"


LNATask::Config::Config()
  : model(0), integration_range(0,0,0)
{
  // pass...
}

LNATask::Config::Config(const Config &other)
  : model(other.model), selected_species(other.selected_species), integrator(other.integrator),
    integration_range(other.integration_range), immediate_steps(other.immediate_steps),
    epsilon_abs(other.epsilon_abs), epsilon_rel(other.epsilon_rel)
{
  // Pass...
}



/* ******************************************************************************************** *
 * Implementation of LNATask
 * ******************************************************************************************** */
LNATask::LNATask(const Config &config, QObject *parent) :
  Task(parent), config(config), interpreter(*(config.model), config.optLevel, config.numThreads, false),
  timeseries(1 + 2*config.selected_species.size() + config.selected_species.size()*(config.selected_species.size()+1)/2,
    1+config.integration_range.getSteps()/(1+config.immediate_steps))
{
  size_t column = 0;

  QVector<QString> species_names(config.selected_species.size());

  this->timeseries.setColumnName(column, "t"); column++;
  for (int i=0; i<config.selected_species.size(); i++, column++)
  {
    QString species_id = config.selected_species.value(i);
    Fluc::Ast::Species *species = config.model->getSpecies(species_id.toStdString());

    if (species->hasName())
      species_names[i] = QString("%1").arg(species->getName().c_str());
    else
      species_names[i] = species_id;

    this->timeseries.setColumnName(column, QString("%1").arg(species_names[i]));
  }

  for (int i=0; i<config.selected_species.size(); i++)
  {
    for (int j=i; j<config.selected_species.size(); j++, column++)
    {
      this->timeseries.setColumnName(
            column,QString("cov(%1,%2)").arg(species_names[i]).arg(species_names[j]));
    }
  }

  for (int i=0; i<config.selected_species.size(); i++, column++)
  {
    this->timeseries.setColumnName(
          column, QString("EMRE(%1)").arg(species_names[i]));
  }

  /*
   * Create integrator:
   */
  switch (config.integrator) {
  case Config::RungeKutta4:
    this->stepper = new Fluc::ODE::RungeKutta4<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.integration_range.getStepSize());
    break;

  case Config::RungeKuttaFehlberg45:
    this->stepper = new Fluc::ODE::RKF45<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.integration_range.getStepSize(),
          config.epsilon_abs, config.epsilon_rel);
    break;

  case Config::DormandPrince5:
    this->stepper = new Fluc::ODE::Dopri853Stepper<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.integration_range.getStepSize(),
          config.epsilon_abs, config.epsilon_rel);
    break;

  case Config::LSODA:
    this->stepper = new Fluc::ODE::LsodaDriver<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.integration_range.getStepSize(),
          config.epsilon_abs, config.epsilon_rel);
    break;

  case Config::Rosenbrock4:
    // First, let LNA Interpreter compile the jacobian
    this->interpreter.compileJacobian();
    // Then, setup integrator:
    this->stepper = new Fluc::ODE::Rosenbrock4TimeInd<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.integration_range.getStepSize(),
          config.epsilon_abs, config.epsilon_rel);
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
  Eigen::VectorXd x(config.model->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(config.model->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(config.model->numSpecies());

  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd cov(config.model->numSpecies(), config.model->numSpecies());

  // Holds EMRE correction for state:
  Eigen::VectorXd emre(config.model->numSpecies());

  // Allocate output vector
  Eigen::VectorXd output_vector(this->timeseries.getNumColumns());

  // Maps the i-th selected species to an index in the concentrations vector:
  size_t N_sel_species = this->config.selected_species.size();
  std::vector<size_t> species_index(N_sel_species);
  for (size_t i=0; i<N_sel_species; i++)
  {
    species_index[i] = this->config.model->getSpeciesIdx(this->config.selected_species.value(i).toStdString());
  }

  // initialize (reduced) state
  config.model->getInitialState(x);
  // get full initial concentrations and covariance
  config.model->fullState(x, concentrations, cov, emre);

  this->setState(Task::RUNNING);

  /*
   * Perform integration
   */
  double t  = config.integration_range.getStartTime();
  double dt = config.integration_range.getStepSize();

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
  for (size_t i=0; i<config.integration_range.getSteps(); i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState())
    {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(i)/config.integration_range.getSteps());

    // Determine update:
    this->stepper->step(x, t, dx);
    // Update state:
    x+=dx;
    // Update time
    t += dt;

    // Skip immediate steps
    if(0 != config.immediate_steps && 0 != i%(1+config.immediate_steps))
      continue;

    // Get full state:
    config.model->fullState(x, concentrations, cov, emre);

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
  return config.selected_species;
}


const Fluc::Ast::Unit &
LNATask::getSpeciesUnit() const
{
  return this->config.model->getConcentrationUnit();
}


const Fluc::Ast::Unit &
LNATask::getTimeUnit() const
{
  return this->config.model->getTimeUnit();
}
