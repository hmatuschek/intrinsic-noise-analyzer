#include "lnatask.hh"
#include "ode/ode.hh"


using namespace iNA;


/* ******************************************************************************************** *
 * Implementation of LNATask
 * ******************************************************************************************** */
LNATask::LNATask(const SSETaskConfig &config, QObject *parent) :
  Task(parent), _config(config), _Ns(config.getModel()->numSpecies()),
  _interpreter(0), _stepper(0),
  _timeseries(1 + 2*_Ns + _Ns*(_Ns+1)/2,
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps())),
  _species_names(_Ns)
{
  size_t column = 0;

  _timeseries.setColumnName(column, "t"); column++;
  for (int i=0; i<(int)_Ns; i++, column++) {
    iNA::Ast::Species *species = config.getModel()->getSpecies(i);
    QString species_id = species->getIdentifier().c_str();

    if (species->hasName()) {
      _species_names[i] = species->getName().c_str();
    } else {
      _species_names[i] = species_id;
    }

    this->_timeseries.setColumnName(column, QString("%1").arg(_species_names[i]));
  }

  for (int i=0; i<(int)_Ns; i++) {
    for (int j=i; j<(int)_Ns; j++, column++) {
      this->_timeseries.setColumnName(
            column,QString("cov(%1,%2)").arg(_species_names[i]).arg(_species_names[j]));
    }
  }

  for (int i=0; i<(int)_Ns; i++, column++)
  {
    this->_timeseries.setColumnName(
          column, QString("EMRE(%1)").arg(_species_names[i]));
  }
}



LNATask::~LNATask()
{
  // Free integrator:
  if (0 != this->_stepper)
    delete _stepper;

  // Free interpreter
  if (0 != _interpreter)
    delete _interpreter;
}


void
LNATask::process()
{
  setState(INITIALIZED, "Compile LNA analysis...");
  setProgress(0.0);
  instantiateInterpreter();

  // Holds the current system state (reduced state)
  Eigen::VectorXd x(_config.getModelAs<iNA::Models::LNAmodel>()->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(_config.getModelAs<iNA::Models::LNAmodel>()->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(_Ns);

  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd cov(_Ns, _Ns);

  // Holds EMRE correction for state:
  Eigen::VectorXd emre(_Ns);

  // Allocate output vector
  Eigen::VectorXd output_vector(this->_timeseries.getNumColumns());

  // initialize (reduced) state
  _config.getModelAs<iNA::Models::LNAmodel>()->getInitialState(x);
  // get full initial concentrations and covariance
  _config.getModelAs<iNA::Models::LNAmodel>()->fullState(x, concentrations, cov, emre);

  setState(Task::RUNNING, "Run analysis...");
  setProgress(0.0);

  /* Perform integration */
  double t  = _config.getIntegrationRange().getStartTime();
  double dt = _config.getIntegrationRange().getStepSize();

  // store initial state RE
  output_vector(0) = t;
  for (size_t j=0; j<_Ns; j++) {
    output_vector(1+j) = concentrations(j);
  }
  // store initial state LNA
  size_t idx = 1 + _Ns;
  for (size_t j=0; j<_Ns; j++) {
    for (size_t k=j; k<_Ns; k++, idx++) {
      output_vector(idx) = cov(j, k);
    }
  }
  // store initial state EMRE
  for (size_t j=0; j<_Ns; j++, idx++) {
    output_vector(idx) = emre(j) + concentrations(j);
  }
  this->_timeseries.append(output_vector);

  // Integration loop:
  for (size_t i=0; i<_config.getIntegrationRange().getSteps(); i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState()) {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(i)/_config.getIntegrationRange().getSteps());

    // Determine update:
    this->_stepper->step(x, t, dx);
    // Update state & time
    x+=dx; t += dt;

    // Skip immediate steps
    if(0 != _config.getIntermediateSteps() && 0 != i%(1+_config.getIntermediateSteps())) {
      continue;
    }

    // Get full state:
    _config.getModelAs<iNA::Models::LNAmodel>()->fullState(x, concentrations, cov, emre);

    // Store new time:
    output_vector(0) = t;

    // Store states of selected species:
    for (size_t j=0; j<_Ns; j++) {
      output_vector(1+j) = concentrations(j);
    }

    // Store cov() of species.
    size_t idx = 1 + _Ns;
    for (size_t j=0; j<_Ns; j++) {
      for (size_t k=j; k<_Ns; k++, idx++) {
        output_vector(idx) = cov(j, k);
      }
    }

    // Store EMRE correction + LNA state:
    for (size_t j=0; j<_Ns; j++, idx++) {
      output_vector(idx) = emre(j) + concentrations(j);
    }

    this->_timeseries.append(output_vector);
  }

  // Finally send last progress event:
  this->setProgress(1.0);
  this->setState(Task::DONE);
}


QString
LNATask::getLabel() {
  return "Time Course Analysis (LNA)";
}


Table *
LNATask::getTimeSeries() {
  return &(this->_timeseries);
}


const QString &
LNATask::getSpeciesName(size_t i) const {
  return _species_names[i];
}

const QVector<QString> &
LNATask::getSpeciesNames() const {
  return _species_names;
}


iNA::Ast::Unit
LNATask::getSpeciesUnit() const {
  return this->_config.getModelAs<iNA::Models::LNAmodel>()->getSpeciesUnit();
}


const iNA::Ast::Unit &
LNATask::getTimeUnit() const {
  return this->_config.getModelAs<iNA::Models::LNAmodel>()->getTimeUnit();
}


void
LNATask::instantiateInterpreter()
{
  switch(_config.getEngine()) {

  /* *********************************** Handle GINAC Engine *********************************** */
  case EngineTaskConfig::GINAC_ENGINE:
    // Assemble REinterpreter with GINAC engine:
    _interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_config.getModelAs<iNA::Models::LNAmodel>(),
          _config.getOptLevel(), _config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (_config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      _stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      _stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      _stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      _stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter)->compileJacobian();

      // Then, setup integrator:
      this->_stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::direct::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::direct::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;
    }
    break;

    /* ******************************** Handle Byte Code Engine ******************************** */
  case EngineTaskConfig::BCI_ENGINE:
    // Assemble REinterpreter with BCI engine:
    _interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_config.getModelAs<iNA::Models::LNAmodel>(),
          _config.getOptLevel(), _config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (_config.getIntegrator()) {
      case SSETaskConfig::RungeKutta4:
        _stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
              _config.getIntegrationRange().getStepSize());
        break;

      case SSETaskConfig::RungeKuttaFehlberg45:
        _stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
              _config.getIntegrationRange().getStepSize(),
              _config.getEpsilonAbs(), _config.getEpsilonRel());
        break;

      case SSETaskConfig::DormandPrince5:
        _stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
              _config.getIntegrationRange().getStepSize(),
              _config.getEpsilonAbs(), _config.getEpsilonRel());
        break;

      case SSETaskConfig::LSODA:
        _stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
              _config.getIntegrationRange().getStepSize(),
              _config.getEpsilonAbs(), _config.getEpsilonRel());
        break;

      case SSETaskConfig::Rosenbrock4:
        // First, let Interpreter compile the jacobian
        static_cast<Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter)->compileJacobian();

        // Then, setup integrator:
        _stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
              *static_cast< Models::GenericSSEinterpreter<
              Models::LNAmodel, Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
              Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
              _config.getIntegrationRange().getStepSize(),
              _config.getEpsilonAbs(), _config.getEpsilonRel());
        break;
      }
    break;

    /* ********************************* Handle BCI OpenMP Engine ********************************* */
  case EngineTaskConfig::BCIMP_ENGINE:
    // Assemble REinterpreter with BCIMP engine:
    _interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_config.getModelAs<iNA::Models::LNAmodel>(),
          _config.getOptLevel(), _config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (_config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      _stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      _stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      _stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      _stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter)->compileJacobian();

      // Then, setup integrator:
      _stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;
    }
    break;


    /* ************************************* Handle JTI Engine ************************************ */
  case EngineTaskConfig::JIT_ENGINE:
    // Assemble REinterpreter with JIT engine:
    _interpreter = new Models::GenericSSEinterpreter<
        Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
        Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(
          *_config.getModelAs<iNA::Models::LNAmodel>(),
          _config.getOptLevel(), _config.getNumEvalThreads(), false);

    // Instantiate integrator for that engine:
    switch (_config.getIntegrator()) {
    case SSETaskConfig::RungeKutta4:
      _stepper = new ODE::RungeKutta4< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize());
      break;

    case SSETaskConfig::RungeKuttaFehlberg45:
      _stepper = new ODE::RKF45< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::DormandPrince5:
      _stepper = new ODE::Dopri5Stepper< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::LSODA:
      _stepper = new ODE::LsodaDriver< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;

    case SSETaskConfig::Rosenbrock4:
      // First, let Interpreter compile the jacobian
      static_cast<Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter)->compileJacobian();

      // Then, setup integrator:
      _stepper = new ODE::Rosenbrock4TimeInd< Models::GenericSSEinterpreter<
          Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > >(
            *static_cast< Models::GenericSSEinterpreter<
            Models::LNAmodel, Eval::jit::Engine<Eigen::VectorXd, Eigen::VectorXd>,
            Eval::jit::Engine<Eigen::VectorXd, Eigen::MatrixXd> > *>(_interpreter),
            _config.getIntegrationRange().getStepSize(),
            _config.getEpsilonAbs(), _config.getEpsilonRel());
      break;
    }
    break;
  }
}
