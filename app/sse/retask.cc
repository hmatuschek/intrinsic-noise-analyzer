#include "retask.hh"
#include "ode/ode.hh"



/* ******************************************************************************************** *
 * Implementation of RETask
 * ******************************************************************************************** */
RETask::RETask(const SSETaskConfig &config, QObject *parent) :
  Task(parent), config(config),
  interpreter(*config.getModelAs<Fluc::Models::REmodel>(), config.getOptLevel(), config.getNumThreads(), false),
  timeseries(1 + config.getNumSpecies(),
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

  /*
   * Create integrator:
   */
  switch (config.getIntegrator()) {
  case SSETaskConfig::RungeKutta4:
    this->stepper = new Fluc::ODE::RungeKutta4<Fluc::Models::REinterpreter >(
          this->interpreter, config.getIntegrationRange().getStepSize());
    break;

  case SSETaskConfig::RungeKuttaFehlberg45:
    this->stepper = new Fluc::ODE::RKF45<Fluc::Models::REinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case SSETaskConfig::DormandPrince5:
    this->stepper = new Fluc::ODE::Dopri5Stepper<Fluc::Models::REinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case SSETaskConfig::LSODA:
    this->stepper = new Fluc::ODE::LsodaDriver<Fluc::Models::REinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case SSETaskConfig::Rosenbrock4:
    // First, let LNA Interpreter compile the jacobian
    this->interpreter.compileJacobian();
    // Then, setup integrator:
    this->stepper = new Fluc::ODE::Rosenbrock4TimeInd<Fluc::Models::REinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;
  }
}



RETask::~RETask()
{
  // Free integrator:
  if (0 != this->stepper)
    delete stepper;
}


void
RETask::process()
{
  // Holds the current system state (reduced state)
  Eigen::VectorXd x(config.getModelAs<Fluc::Models::REmodel>()->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(config.getModelAs<Fluc::Models::REmodel>()->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(config.getModel()->numSpecies());

  // Holds a row of the output-table:
  Eigen::VectorXd output_vector(1 + config.getNumSpecies());

  // Maps the i-th selected species to an index in the concentrations vector:
  size_t N_sel_species = this->config.getNumSpecies();
  std::vector<size_t> species_index(N_sel_species);
  for (size_t i=0; i<N_sel_species; i++)
  {
    species_index[i] = this->config.getModel()->getSpeciesIdx(
          this->config.getSelectedSpecies().value(i).toStdString());
  }

  // initialize (reduced) state
  config.getModelAs<Fluc::Models::REmodel>()->getInitialState(x);
  // get full initial concentrations and covariance
  config.getModelAs<Fluc::Models::REmodel>()->fullState(x, concentrations);

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
  this->timeseries.append(output_vector);

  // Integration loop:
  size_t N_steps = config.getIntegrationRange().getSteps();
  size_t N_intermediate = config.getIntermediateSteps();
  for (size_t i=0; i<N_steps; i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState())
    {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(i)/N_steps);

    // Determine update:
    this->stepper->step(x, t, dx);
    // Update state:
    x+=dx;
    // Update time
    t += dt;

    // Skip immediate steps
    if(0 != N_intermediate && 0 != i%(1+N_intermediate))
      continue;

    // Get full state:
    config.getModelAs<Fluc::Models::REmodel>()->fullState(x, concentrations);

    // Store new time:
    output_vector(0) = t;

    // Store states of selected species:
    for (size_t j=0; j<N_sel_species; j++)
    {
      size_t index_i = species_index[j];
      output_vector(1+j) = concentrations(index_i);
    }

    this->timeseries.append(output_vector);
  }

  // Finally send last progress event:
  this->setProgress(1.0);
  this->setState(Task::DONE);
}


QString
RETask::getLabel()
{
  return "Time Course Analysis (RE)";
}


Table *
RETask::getTimeSeries()
{
  return &(this->timeseries);
}


const QList<QString> &
RETask::getSelectedSpecies() const
{
  return config.getSelectedSpecies();
}


const Fluc::Ast::Unit &
RETask::getSpeciesUnit() const
{
  return this->config.getModelAs<Fluc::Models::REmodel>()->getConcentrationUnit();
}


const Fluc::Ast::Unit &
RETask::getTimeUnit() const
{
  return this->config.getModelAs<Fluc::Models::REmodel>()->getTimeUnit();
}
