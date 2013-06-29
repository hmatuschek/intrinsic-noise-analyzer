#include "iostask.hh"
#include "ode/ode.hh"
#include "utils/logger.hh"

using namespace iNA;


/* ******************************************************************************************** *
 * Implementation of IOSTask
 * ******************************************************************************************** */
IOSTask::IOSTask(const SSETaskConfig &config, QObject *parent) :
  Task(parent), config(config), _Ns(config.getModel()->numSpecies()),
  interpreter(0),
  timeseries(
    1 + 3*_Ns + _Ns*(_Ns+1),
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps())),
  species_names(_Ns), re_index_table(_Ns), lna_index_table(_Ns, _Ns), emre_index_table(_Ns),
  ios_index_table(_Ns, _Ns), ios_emre_index_table(_Ns),
  has_negative_variance(false)
{

  _sseModel = new iNA::Models::IOSmodel(*config.getModel());

  // Assemble index tables and assign column names to time-series table:
  size_t column = 0;

  // Time column:
  this->timeseries.setColumnName(column, "t"); column++;

  // RE means and initialize vector of species names
  for (int i=0; i<(int)_Ns; i++, column++) {
    iNA::Ast::Species *species = config.getModel()->getSpecies(i);
    QString species_id = species->getIdentifier().c_str();
    if (species->hasName()) {
      species_names[i] = species->getName().c_str();
    } else {
      species_names[i] = species_id;
    }

    this->timeseries.setColumnName(column, QString("RE %1").arg(species_names[i]));
    this->re_index_table(i) = column;
  }

  // LNA Covariance
  for (int i=0; i<(int)_Ns; i++) {
    for (int j=i; j<(int)_Ns; j++, column++) {
      this->timeseries.setColumnName(
            column, QString("LNA cov(%1,%2)").arg(species_names[i]).arg(species_names[j]));
      this->lna_index_table(i,j) = column;
      this->lna_index_table(j,i) = column;
    }
  }

  // EMRE means
  for (int i=0; i<(int)_Ns; i++, column++) {
    this->timeseries.setColumnName(
          column, QString("EMRE %1").arg(species_names[i]));
    this->emre_index_table(i) = column;
  }

  // IOS Covariance
  for (int i=0; i<(int)_Ns; i++) {
    for (int j=i; j<(int)_Ns; j++, column++) {
      this->timeseries.setColumnName(
            column, QString("IOS cov(%1,%2)").arg(species_names[i]).arg(species_names[j]));
      this->ios_index_table(i,j) = column;
      this->ios_index_table(j,i) = column;
    }
  }

  // IOS EMRE Mean corrections
  for (int i=0; i<(int)_Ns; i++, column++) {
    this->timeseries.setColumnName(
          column, QString("IOS %1").arg(species_names[i]));
    this->ios_emre_index_table(i) = column;
  }
}



IOSTask::~IOSTask()
{
  // Free integrator:
  if (0 != this->stepper)
    delete stepper;

  // Free interpreter
  if (0 != this->interpreter)
    delete interpreter;

  // Free sseModel
  if (0 != this->_sseModel)
    delete _sseModel;


}


void
IOSTask::process()
{
  this->setState(Task::INITIALIZED);
  this->setProgress(0.0);

  {
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Compiling IOS time course analysis.";
    Utils::Logger::get().log(message);
  }
  instantiateInterpreter();

  {
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Starting IOS time course analysis.";
    Utils::Logger::get().log(message);
  }

  // Holds the current system state (reduced state)
  Eigen::VectorXd x(_sseModel->getDimension());
  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(_Ns);
  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd lna(_Ns, _Ns);
  // Holds EMRE correction for state:
  Eigen::VectorXd emre(_Ns);
  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd ios(_Ns, _Ns);
  // Holds the 3rd moment for each species.
  Eigen::VectorXd thirdMoment = Eigen::VectorXd::Zero(_Ns);

  // Holds the iosemre for each species.
  Eigen::VectorXd iosemre(_Ns);
  // Holds a row of the output-table:
  Eigen::VectorXd output_vector(timeseries.getNumColumns());

  // initialize (reduced) state
  _sseModel->getInitialState(x);
  // get full initial concentrations and covariance
  _sseModel->fullState(
        x, concentrations, lna, emre, ios, thirdMoment, iosemre);

  this->setState(Task::RUNNING);
  this->setProgress(0.0);

  /*
   * Perform integration
   */
  double t  = config.getIntegrationRange().getStartTime();
  double dt = config.getIntegrationRange().getStepSize();

  // store initial state:
  output_vector(0) = t;
  for (size_t i=0; i<_Ns; i++)
  {
    output_vector(re_index_table(i)) = concentrations(i);
    output_vector(emre_index_table(i)) = concentrations(i);
    output_vector(ios_emre_index_table(i)) = concentrations(i);
    for (size_t j=i; j<_Ns; j++){
      output_vector(lna_index_table(i,j)) = 0.0;
      output_vector(ios_index_table(i,j)) = 0.0;
    }
  }
  this->timeseries.append(output_vector);

  // Integration loop:
  size_t N_steps = config.getIntegrationRange().getSteps();
  size_t N_intermediate = config.getIntermediateSteps();
  for (size_t s=0; s<N_steps; s++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState()) {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(s)/N_steps);

    // Update state & time
    this->stepper->step(x, t); t += dt;

    // Skip immediate steps
    if(0 != N_intermediate && 0 != s%(1+N_intermediate)) {
      continue;
    }

    // Get full state:
    _sseModel->fullState(
          x, concentrations, lna, emre, ios, thirdMoment, iosemre);

    // store state and time:
    output_vector(0) = t;
    for (size_t i=0; i<_Ns; i++) {
      // Store means
      output_vector(re_index_table(i)) = concentrations(i);
      output_vector(emre_index_table(i)) = emre(i) + concentrations(i);
      output_vector(ios_emre_index_table(i)) = emre(i) + concentrations(i) + iosemre(i);
      // Store covariances:
      for (size_t j=i; j<_Ns; j++){
        output_vector(lna_index_table(i,j)) = lna(i, j);
        output_vector(ios_index_table(i,j)) = lna(i, j) + ios(i, j);

        // Check if one of the variance elements is negative:
        if ( (i == j) && (0.0 > output_vector(lna_index_table(i,i)) || (0.0 > ios_index_table(i,i)))) {
          this->has_negative_variance = true;
        }
      }
    }
    this->timeseries.append(output_vector);
  }

  // Finally send last progress event:
  this->setProgress(1.0);
  this->setState(Task::DONE);

  {
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Finished IOS time course analysis.";
    Utils::Logger::get().log(message);
  }
}


QString
IOSTask::getLabel() {
  return "Time Course Analysis (IOS)";
}


Table *
IOSTask::getTimeSeries() {
  return &(this->timeseries);
}


const QString &
IOSTask::getSpeciesName(size_t i) const {
  return species_names[i];
}

const QVector<QString> &
IOSTask::getSpeciesNames() const {
  return species_names;
}


iNA::Ast::Unit
IOSTask::getSpeciesUnit() const {
  return this->_sseModel->getSpeciesUnit();
}


iNA::Ast::Unit
IOSTask::getTimeUnit() const {
  return this->_sseModel->getTimeUnit();
}

bool
IOSTask::hasNegativeVariance() const {
  return this->has_negative_variance;
}


void
IOSTask::instantiateInterpreter()
{
  /*
   * First, construct interpreter and integerator by selected execution engine:
   */
  switch(config.getEngine()) {

  /* *********************************** Handle GINAC Engine *********************************** */
  case EngineTaskConfig::GINAC_ENGINE:
    // Assemble REinterpreter with GINAC engine:
    interpreter = new Models::GenericSSEinterpreter<
        Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_sseModel,
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      this->stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
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
        Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_sseModel,
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
      case SSETaskConfig::RungeKutta4:
        stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize());
        break;

      case SSETaskConfig::RungeKuttaFehlberg45:
        stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::DormandPrince5:
        stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::LSODA:
        stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
              config.getIntegrationRange().getStepSize(),
              config.getEpsilonAbs(), config.getEpsilonRel());
        break;

      case SSETaskConfig::Rosenbrock4:
        // First, let Interpreter compile the jacobian
        static_cast<Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

        // Then, setup integrator:
        stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::IOSmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
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
        Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_sseModel,
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
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
        Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_sseModel,
          config.getOptLevel(), config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter)->compileJacobian();

      // Then, setup integrator:
      stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::IOSmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(interpreter),
            config.getIntegrationRange().getStepSize(),
            config.getEpsilonAbs(), config.getEpsilonRel());
      break;
    }
    break;
  }
}
