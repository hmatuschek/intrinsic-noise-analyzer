#include "retask.hh"
#include "ode/ode.hh"

using namespace iNA;


/*
 * By the way, have I ever told you how much I hate, I mean really hate, the C++ template syntax?
 */



/* ******************************************************************************************** *
 * Implementation of RETask
 * ******************************************************************************************** */
RETask::RETask(const SSETaskConfig &config, QObject *parent) :
  Task(parent), config(config),
  interpreter(0), stepper(0),
  timeseries(1 + config.getNumSpecies(),
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps()))
{
  // First, construct interpreter and integerator by selected execution engine:
  switch(config.getEngine()) {

  /* *********************************** Handle GINAC Engine *********************************** */
  case EngineTaskConfig::GINAC_ENGINE:
    // Assemble REinterpreter with GINAC engine:
    interpreter = new Models::GenericSSEinterpreter<
        Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<iNA::Models::REmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      this->stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
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
          Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
            *config.getModelAs<iNA::Models::REmodel>(),
            config.getOptLevel(), config.getNumEvalThreads(), false);

      // Instantiate integrator for that engine:
      switch (config.getIntegrator()) {
      case SSETaskConfig::RungeKutta4:
        stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize());
        break;

      case SSETaskConfig::RungeKuttaFehlberg45:
        stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::DormandPrince5:
        stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::LSODA:
        stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::Rosenbrock4:
        // First, let Interpreter compile the jacobian
        static_cast<Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

        // Then, setup integrator:
        stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::REmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
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
        Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<iNA::Models::REmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
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
        Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *config.getModelAs<iNA::Models::REmodel>(),
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::REmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;
    }
    break;
  }


  size_t column = 0;
  QVector<QString> species_names(config.getNumSpecies());
  this->timeseries.setColumnName(column, "t"); column++;
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
  {
    QString species_id = config.getSelectedSpecies().value(i);
    iNA::Ast::Species *species = config.getModel()->getSpecies(species_id.toStdString());

    if (species->hasName())
      species_names[i] = QString("%1").arg(species->getName().c_str());
    else
      species_names[i] = species_id;

    this->timeseries.setColumnName(column, QString("%1").arg(species_names[i]));
  }
}



RETask::~RETask()
{
  // Free integrator:
  if (0 != this->stepper)
    delete stepper;

  // Free interpreter
  if (0 != this->interpreter)
    delete interpreter;
}


void
RETask::process()
{
  // Holds the current system state (reduced state)
  Eigen::VectorXd x(config.getModelAs<iNA::Models::REmodel>()->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(config.getModelAs<iNA::Models::REmodel>()->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(config.getModel()->numSpecies());

  // Holds a row of the output-table:
  Eigen::VectorXd output_vector(1 + config.getModel()->numSpecies());

  // Maps the i-th selected species to an index in the concentrations vector:
  size_t N_species = config.getModel()->numSpecies();

  // initialize (reduced) state
  config.getModelAs<iNA::Models::REmodel>()->getInitialState(x);
  // get full initial concentrations and covariance
  config.getModelAs<iNA::Models::REmodel>()->fullState(x, concentrations);

  this->setState(Task::RUNNING);

  /*
   * Perform integration
   */
  double t  = config.getIntegrationRange().getStartTime();
  double dt = config.getIntegrationRange().getStepSize();

  // store initial state:
  output_vector(0) = t;
  for (size_t j=0; j<N_species; j++) {
    output_vector(1+j) = concentrations(j);
  }
  this->timeseries.append(output_vector);

  // Integration loop:
  size_t N_steps = config.getIntegrationRange().getSteps();
  size_t N_intermediate = config.getIntermediateSteps();
  for (size_t i=0; i<N_steps; i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState()) {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(i)/N_steps);

    // Determine update:
    this->stepper->step(x, t, dx);

    // Update state & time
    x+=dx; t += dt;

    // Skip immediate steps
    if(0 != N_intermediate && 0 != i%(1+N_intermediate))
      continue;

    // Get full state:
    config.getModelAs<iNA::Models::REmodel>()->fullState(x, concentrations);

    // Store new time:
    output_vector(0) = t;
    // Store states of selected species:
    for (size_t j=0; j<N_species; j++) {
      output_vector(1+j) = concentrations(j);
    }

    this->timeseries.append(output_vector);
  }

  // Finally send last progress event:
  this->setProgress(1.0);
  this->setState(Task::DONE);
}


QString
RETask::getLabel() {
  return "Time Course Analysis (RE)";
}


const SSETaskConfig &
RETask::getConfig() const {
  return config;
}


Table *
RETask::getTimeSeries()
{
  return &(this->timeseries);
}


iNA::Ast::Unit
RETask::getSpeciesUnit() const
{
  return this->config.getModelAs<iNA::Models::REmodel>()->getConcentrationUnit();
}


const iNA::Ast::Unit &
RETask::getTimeUnit() const
{
  return this->config.getModelAs<iNA::Models::REmodel>()->getTimeUnit();
}
