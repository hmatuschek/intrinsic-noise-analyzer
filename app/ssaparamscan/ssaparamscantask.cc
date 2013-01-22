#include "ssaparamscantask.hh"
#include "models/ssasteadystate.hh"

/* ******************************************************************************************* *
 * Implementation of ParamScanTask::Config, the task configuration.
 * ******************************************************************************************* */
SSAParamScanTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(),
    _model(0), num_threads(0), t_transient(0), t_max(0), timestep(0),
    parameter(), start_value(0), end_value(1),
    steps(1)
{
  // Pass...
}

SSAParamScanTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other),
    _model(other._model), num_threads(other.num_threads),
    parameter(other.parameter), start_value(other.start_value), end_value(other.end_value), steps(other.steps),
    t_transient(other.t_transient), t_max(other.t_max), timestep(other.timestep)
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
    : Task(parent), config(config), _Ns(config.getModel()->numSpecies()), parameterScan(1,1)
{

    std::vector<QString> species_name(_Ns);
    // Make space
    parameterScan.resize(1+_Ns+_Ns*(_Ns+1)/2, config.getSteps()+1);

    size_t column = 0;

    // first column parameter name
    this->parameterScan.setColumnName(column++, config.getParameter().hasName() ? config.getParameter().getName().c_str() : config.getParameter().getIdentifier().c_str() );

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

    for (size_t i=0; i<_Ns; i++)
      for (size_t j=0; j<=i; j++, column++)
        this->parameterScan.setColumnName(
              column,QString("cov(%1,%2)").arg(species_name[i]).arg(species_name[j]));

}


void
SSAParamScanTask::process()
{

  this->setState(Task::INITIALIZED);
  this->setProgress(0);

  // Construct parameter sets
  std::vector<iNA::Models::ParameterSet> parameterSets(config.getSteps()+1);
  for(size_t j = 0; j<=config.getSteps(); j++)
  {
      double val = config.getStartValue()+config.getInterval()*j;
      parameterSets[j].insert(std::pair<std::string,double>(config.getParameter().getIdentifier(),val));
  }

  // Allocate result matrices
  Eigen::MatrixXd mean,cov;

  // and perform analysis
  iNA::Models::SSAparamScan pscan(dynamic_cast<iNA::Ast::Model &>(*config.getModel()),
                                  parameterSets, config.getTransientTime());

  for(double t=0;t<=config.getMaxTime(); t+=config.getTimeStep())
  {
      pscan.run(config.getTimeStep());

      /*
      // Fill table
      for(size_t pid=0; pid<parameterSets.size(); pid++)
      {

          parameterScan(pid,0) = GiNaC::ex_to<GiNaC::numeric>(parameterSets[pid][config.getParameter().getIdentifier()]).to_double();

          int col=1;

          // output means
          for (size_t i=0; i<_Ns; i++)
              parameterScan(pid,col++) = pscan.getMean()(pid,i);

          // output covariances
          int idx = 0;
          for (size_t i=0; i<_Ns; i++)
            for (size_t j=0; j<=i; j++)
                parameterScan(pid,col++) =  pscan.getCovariance()(pid,idx++);

      }

      */

      //////////////////////////////////////////////////////////
      // Should show a preview for the data of parameterScan here every timestep or less.
      //////////////////////////////////////////////////////////


  }

  // Check if task shall terminate:
  if (Task::TERMINATING == this->getState())
  {
    this->setState(Task::ERROR, tr("Task was terminated by user."));
    return;
  }

  // Fill table
  for(size_t pid=0; pid<parameterSets.size(); pid++)
  {

      parameterScan(pid,0) = GiNaC::ex_to<GiNaC::numeric>(parameterSets[pid][config.getParameter().getIdentifier()]).to_double();

      int col=1;

      // output means
      for (size_t i=0; i<_Ns; i++)
          parameterScan(pid,col++) = pscan.getMean()(pid,i);

      // output covariances
      int idx = 0;
      for (size_t i=0; i<_Ns; i++)
        for (size_t j=0; j<=i; j++)
            parameterScan(pid,col++) =  pscan.getCovariance()(pid,idx++);

  }

  // Done...
  this->setState(Task::DONE);

  {
   iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
   message << "Finished parameter scan.";
   iNA::Utils::Logger::get().log(message);
  }

}


const SSAParamScanTask::Config &
SSAParamScanTask::getConfig() const {
  return config;
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
