#include "lnatask.hh"
#include "ode/ode.hh"


using namespace Fluc;


/* ******************************************************************************************** *
 * Implementation of LNATask
 * ******************************************************************************************** */
LNATask::LNATask(const SSETaskConfig &config, QObject *parent) :
  Task(parent), config(config),
  interpreter(0), stepper(0),
  timeseries(1 + 2*config.getNumSpecies() + config.getNumSpecies()*(config.getNumSpecies()+1)/2,
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps())),
  species_names(config.getNumSpecies())
{
  size_t column = 0;

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
}



LNATask::~LNATask()
{
  // Free integrator:
  if (0 != this->stepper)
    delete stepper;

  // Free interpreter
  if (0 != interpreter)
    delete interpreter;
}


void
LNATask::process()
{
  setState(INITIALIZED, "Compile LNA analysis...");
  setProgress(0.0);
  instantiateInterpreter();

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


  setState(Task::RUNNING, "Run analysis...");
  setProgress(0.0);

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
  return "Time Course Analysis (LNA)";
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

const QString &
LNATask::getSpeciesName(size_t i) const
{
  return species_names[i];
}

const QVector<QString> &
LNATask::getSpeciesNames() const
{
  return species_names;
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


void
LNATask::instantiateInterpreter()
{
  switch(config.getEngine()) {

  /* *********************************** Handle GINAC Engine *********************************** */
  case EngineTaskConfig::GINAC_ENGINE:
    // Assemble REinterpreter with GINAC engine:
    interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<Fluc::Models::LNAmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      this->stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;
    }
    break;

    /* ******************************** Handle Byte Code Engine ******************************** */
  case EngineTaskConfig::BCI_ENGINE:
    // Assemble REinterpreter with BCI engine:
    interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<Fluc::Models::LNAmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
      case SSETaskConfig::RungeKutta4:
        stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize());
        break;

      case SSETaskConfig::RungeKuttaFehlberg45:
        stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::DormandPrince5:
        stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::LSODA:
        stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::Rosenbrock4:
        // First, let Interpreter compile the jacobian
        static_cast<Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

        // Then, setup integrator:
        stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;
      }
    break;

    /* ********************************* Handle BCI OpenMP Engine ********************************* */
  case EngineTaskConfig::BCIMP_ENGINE:
    // Assemble REinterpreter with BCIMP engine:
    interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<Fluc::Models::LNAmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;
    }
    break;


    /* ************************************* Handle JTI Engine ************************************ */
  case EngineTaskConfig::JIT_ENGINE:
    // Assemble REinterpreter with JIT engine:
    interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<Fluc::Models::LNAmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;
    }
    break;
  }
}
