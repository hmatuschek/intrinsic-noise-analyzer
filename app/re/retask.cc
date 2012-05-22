#include "retask.hh"
#include "ode/ode.hh"


RETask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), SpeciesSelectionTaskConfig(),
    ODEIntTaskConfig(), model(0)
{
  // pass...
}

RETask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), SpeciesSelectionTaskConfig(other),
    ODEIntTaskConfig(other), model(other.model)
{
  // Pass...
}


void
RETask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);
  // Construct LNA model from SBML model associated with the selected document
  this->model = new Fluc::Models::LinearNoiseApproximation(document->getSBMLModel());
}


Fluc::Ast::Model *
RETask::Config::getModel() const
{
  return this->model;
}



/* ******************************************************************************************** *
 * Implementation of LNATask
 * ******************************************************************************************** */
RETask::RETask(const Config &config, QObject *parent) :
  Task(parent), config(config),
  interpreter(*(config.model), config.getOptLevel(), config.getNumThreads(), false),
  timeseries(1 + config.getNumSpecies(),
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps()))
{
  size_t column = 0;

  QVector<QString> species_names(config.getNumSpecies());
  this->timeseries.setColumnName(column, "t"); column++;
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
  {
    QString species_id = config.getSelectedSpecies().value(i);
    Fluc::Ast::Species *species = config.model->getSpecies(species_id.toStdString());

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
  case Config::RungeKutta4:
    this->stepper = new Fluc::ODE::RungeKutta4<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize());
    break;

  case Config::RungeKuttaFehlberg45:
    this->stepper = new Fluc::ODE::RKF45<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case Config::DormandPrince5:
    this->stepper = new Fluc::ODE::Dopri5Stepper<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case Config::LSODA:
    this->stepper = new Fluc::ODE::LsodaDriver<Fluc::Models::LNAinterpreter>(
          this->interpreter, config.getIntegrationRange().getStepSize(),
          config.getEpsilonAbs(), config.getEpsilonRel());
    break;

  case Config::Rosenbrock4:
    // First, let LNA Interpreter compile the jacobian
    this->interpreter.compileJacobian();
    // Then, setup integrator:
    this->stepper = new Fluc::ODE::Rosenbrock4TimeInd<Fluc::Models::LNAinterpreter>(
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
  Eigen::VectorXd x(config.model->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(config.model->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(config.model->numSpecies());

  // Holds a row of the output-table:
  Eigen::VectorXd output_vector(1 + config.getNumSpecies());

  // Maps the i-th selected species to an index in the concentrations vector:
  size_t N_sel_species = this->config.getNumSpecies();
  std::vector<size_t> species_index(N_sel_species);
  for (size_t i=0; i<N_sel_species; i++)
  {
    species_index[i] = this->config.model->getSpeciesIdx(this->config.getSelectedSpecies().value(i).toStdString());
  }

  // initialize (reduced) state
  config.model->getInitialState(x);
  // get full initial concentrations and covariance
  config.model->fullState(x, concentrations);

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
    config.model->fullState(x, concentrations);

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
  return this->config.model->getConcentrationUnit();
}


const Fluc::Ast::Unit &
RETask::getTimeUnit() const
{
  return this->config.model->getTimeUnit();
}
