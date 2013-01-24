#include "ssaparamscantask.hh"
#include "models/ssaparamscan.hh"

/* ******************************************************************************************* *
 * Implementation of ParamScanTask::Config, the task configuration.
 * ******************************************************************************************* */
SSAParamScanTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(),
    _model(0),
    parameter(), start_value(0), end_value(1), steps(1),
    t_transient(0), t_max(0), timestep(0),
    num_threads(0)
{
  // Pass...
}

SSAParamScanTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other),
    _model(other._model),
    parameter(other.parameter), start_value(other.start_value), end_value(other.end_value), steps(other.steps),
    t_transient(other.t_transient), t_max(other.t_max), timestep(other.timestep),
    num_threads(other.num_threads)
{
  // Pass...
}

void
SSAParamScanTask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);

  // Try to construct SSA model from SBML model associated with the selected document
  new iNA::Models::OptimizedSSA(document->getModel(),1, 1024);

  this->_model = &(document->getModel());

  ModelSelectionTaskConfig::setModelDocument(document);

}

iNA::Ast::Model *SSAParamScanTask::Config::getModel() const
{
    return this->_model;
}

void
SSAParamScanTask::Config::setNumThreads(size_t num)
{
  num_threads = num;
}

size_t
SSAParamScanTask::Config::getNumThreads() const
{
    return this->num_threads;
}

double
SSAParamScanTask::Config::getMaxTime() const
{
  return this->t_max;
}

void
SSAParamScanTask::Config::setMaxTime(double t_max)
{
  this->t_max = t_max;
}

double
SSAParamScanTask::Config::getTimeStep() const
{
  return timestep;
}

void
SSAParamScanTask::Config::setTimeStep(double timestep)
{
  this->timestep = timestep;
}

double
SSAParamScanTask::Config::getTransientTime() const
{
  return this->t_transient;
}

void
SSAParamScanTask::Config::setTransientTime(double t)
{
  this->t_transient = t;
}



/* ******************************************************************************************* *
 * Implementation of ParamScanTask::Config, the task configuration.
 * ******************************************************************************************* */
SSAParamScanTask::SSAParamScanTask(const Config &config, QObject *parent)
    : Task(parent), config(config), _Ns(config.getModel()->numSpecies()), parameterScan(1,1),
      _is_initialized(false), _is_final(false), _pscan(0), _current_time(0.0)
{
  // Will hold the species names
  std::vector<QString> species_name(_Ns);
  // Make space for result table
  parameterScan.resize(1+_Ns+_Ns*(_Ns+1)/2, config.getSteps()+1);

  size_t column = 0;
  // first column parameter name
  QString name = config.getParameter().getIdentifier().c_str();
  if (config.getParameter().hasName()) { name = config.getParameter().getName().c_str(); }
  this->parameterScan.setColumnName(column++, name);

  for (size_t i=0; i<_Ns; i++, column++)
  {
    // fill index table
    QString species_id = config.getModel()->getSpecies(i)->getIdentifier().c_str();
    iNA::Ast::Species *species = config.getModel()->getSpecies(species_id.toStdString());

    if (species->hasName())
      species_name[i] = QString("%1").arg(species->getName().c_str());
    else
      species_name[i] = species_id;

    this->parameterScan.setColumnName(column, QString("%1").arg(species_name[i]));
  }

  for (size_t i=0; i<_Ns; i++) {
    for (size_t j=0; j<=i; j++, column++) {
      this->parameterScan.setColumnName(
          column,QString("cov(%1,%2)").arg(species_name[i]).arg(species_name[j]));
    }
  }
}


SSAParamScanTask::~SSAParamScanTask()
{
  // Free parameter scan if present:
  if (0 != _pscan) {
    delete _pscan;
  }
}


void
SSAParamScanTask::process()
{
  // If task shall terminate -> return immediately
  if ( Task::TERMINATING == getState() ) {
    setState(Task::ERROR, tr("Task was terminated by the user."));
    return;
  }
  // If task runs, or is finally done (or in error state)
  if ( (Task::ERROR == getState()) || (Task::DONE == getState()) || (Task::RUNNING == getState())) {
    return;
  }

  // Initialize scan if not done yet:
  if (! _is_initialized) {
    initializeScan();
  }

  // perform a step:
  performStep();
}


void
SSAParamScanTask::initializeScan()
{
  this->setState(Task::INITIALIZED);
  this->setProgress(0);

  // Construct parameter sets
  _parameterSets.resize(config.getSteps()+1);
  for(size_t j = 0; j<=config.getSteps(); j++)
  {
    double val = config.getStartValue()+config.getInterval()*j;
    _parameterSets[j].insert(
          std::pair<std::string,double>(config.getParameter().getIdentifier(),val));
  }

  // Construct analysis:
  _pscan = new iNA::Models::SSAparamScan(dynamic_cast<iNA::Ast::Model &>(*config.getModel()),
                                         _parameterSets, config.getTransientTime(),
                                         config.getNumThreads());

  // Set simulation time.
  _current_time = 0.0;

  // mark analysis being initialized
  _is_initialized = true;
}


void
SSAParamScanTask::performStep()
{
  // Restart
  this->setState(Task::INITIALIZED);
  this->setProgress(0);

  // perform a step:
  _pscan->run(config.getTimeStep());
  _current_time += config.getTimeStep();

  // Check if task shall terminate:
  if (Task::TERMINATING == this->getState())
  {
    this->setState(Task::ERROR, tr("Task was terminated by user."));
    return;
  }

  // Fill table
  for(size_t pid=0; pid<_parameterSets.size(); pid++)
  {
    parameterScan(pid,0) = GiNaC::ex_to<GiNaC::numeric>(
          _parameterSets[pid][config.getParameter().getIdentifier()]).to_double();

    int col=1;

    // output means
    for (size_t i=0; i<_Ns; i++)
      parameterScan(pid,col++) = _pscan->getMean()(pid,i);

    // output covariances
    int idx = 0;
    for (size_t i=0; i<_Ns; i++)
      for (size_t j=0; j<=i; j++)
        parameterScan(pid,col++) =  _pscan->getCovariance()(pid,idx++);
  }

  // Done...
  this->setState(Task::DONE);
}


const SSAParamScanTask::Config &
SSAParamScanTask::getConfig() const {
  return config;
}


bool
SSAParamScanTask::isFinal() const {
  return _is_final;
}

void
SSAParamScanTask::setFinal(bool final) {
  _is_final = final;
  this->setState(Task::DONE);
}


QString
SSAParamScanTask::getLabel()
{
  return "Parameter Scan (SSA)";
}


iNA::Ast::Unit
SSAParamScanTask::getSpeciesUnit() const
{
  return config.getModel()->getSpeciesUnit();
}


Table & SSAParamScanTask::getParameterScan() { return parameterScan; }
