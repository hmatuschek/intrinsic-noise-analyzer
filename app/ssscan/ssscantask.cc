#include "ssscantask.hh"

/* ******************************************************************************************* *
 * Implementation of ParamScanTask::Config, the task configuration.
 * ******************************************************************************************* */
ParamScanTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), SpeciesSelectionTaskConfig(),
    _model(0), num_threads(0), max_iterations(0), max_time_step(0), epsilon(0),
    parameter(), start_value(0), end_value(1),
    steps(1)
{
  // Pass...
}

ParamScanTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), SpeciesSelectionTaskConfig(other),
    _model(other._model), num_threads(other.num_threads), max_iterations(other.max_iterations), max_time_step(other.max_time_step),
    epsilon(other.epsilon),
    parameter(other.parameter), start_value(other.start_value), end_value(other.end_value),
    steps(other.steps)
{
  // Pass...
}

void
ParamScanTask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);
  // Construct IOS model from SBML model associated with the selected document
  this->_model = new iNA::Models::IOSmodel(document->getModel());
}

iNA::Ast::Model *
ParamScanTask::Config::getModel() const
{
  return _model;
}

void
ParamScanTask::Config::setNumThreads(size_t num)
{
  num_threads = num;
}

size_t
ParamScanTask::Config::getMaxIterations() const
{
  return this->max_iterations;
}

void
ParamScanTask::Config::setMaxIterations(size_t num)
{
  this->max_iterations = num;
}

double
ParamScanTask::Config::getMaxTimeStep() const
{
  return max_time_step;
}

void
ParamScanTask::Config::setMaxTimeStep(double t_max)
{
  max_time_step = t_max;
}

double
ParamScanTask::Config::getEpsilon() const
{
  return this->epsilon;
}

void
ParamScanTask::Config::setEpsilon(double eps)
{
  this->epsilon = eps;
}



/* ******************************************************************************************* *
 * Implementation of ParamScanTask::Config, the task configuration.
 * ******************************************************************************************* */
ParamScanTask::ParamScanTask(const Config &config, QObject *parent)
  : Task(parent), config(config),
    steady_state(dynamic_cast<iNA::Models::IOSmodel &>(*config.getModel()),
      config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep()),
    species_name(config.getNumSpecies()),
    parameterScan(1+2*config.getNumSpecies()+config.getNumSpecies()*(config.getNumSpecies()+1), config.getSteps()+1),
    index_table(config.getNumSpecies())
{

    size_t column = 0;

    // first column parameter name
    this->parameterScan.setColumnName(column++, config.getParameter().hasName() ? config.getParameter().getName().c_str() : config.getParameter().getIdentifier().c_str() );

    for (size_t i=0; i<config.getNumSpecies(); i++, column++)
    {
      // fill index table
      QString species_id = config.getSelectedSpecies().value(i);
      iNA::Ast::Species *species = config.getModel()->getSpecies(species_id.toStdString());      
      this->index_table[i] = config.getModel()->getSpeciesIdx(species_id.toStdString());

      if (species->hasName())
        species_name[i] = QString("%1").arg(species->getName().c_str());
      else
        species_name[i] = species_id;

      this->parameterScan.setColumnName(column, QString("%1").arg(species_name[i]));
    }

    for (int i=0; i<(int)config.getNumSpecies(); i++)
      for (int j=i; j<(int)config.getNumSpecies(); j++, column++)
        this->parameterScan.setColumnName(
              column,QString("cov(%1,%2)").arg(species_name[i]).arg(species_name[j]));

    for (size_t i=0; i<config.getNumSpecies(); i++, column++)
      this->parameterScan.setColumnName(
            column, QString("EMRE(%1)").arg(species_name[i]));

    for (size_t i=0; i<config.getNumSpecies(); i++)
      for (size_t j=i; j<config.getNumSpecies(); j++, column++)
        this->parameterScan.setColumnName(
              column,QString("cov(%1,%2)").arg(species_name[i]).arg(species_name[j]));

}



void
ParamScanTask::process()
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

  // Take model
  iNA::Models::IOSmodel *model
      = dynamic_cast<iNA::Models::IOSmodel *>(config.getModel());

  // Allocate result matrix (of unified state vectors)
  std::vector<Eigen::VectorXd> scanResult(model->getDimension());

  // Do parameter scan
  this->steady_state.parameterScan(parameterSets,scanResult);

  // Check if task shall terminate:
  if (Task::TERMINATING == this->getState())
  {
    this->setState(Task::ERROR, tr("Task was terminated by user."));
    return;
  }


  // Fill table
  for(size_t pid=0; pid<scanResult.size(); pid++)
  {
      // Some temporary vectors for the result of the analysis
      Eigen::VectorXd concentrations(config.getModel()->numSpecies());
      Eigen::VectorXd emre_corrections(config.getModel()->numSpecies());
      Eigen::VectorXd iosemre_corrections(config.getModel()->numSpecies());
      Eigen::MatrixXd lna_covariances(config.getModel()->numSpecies(), config.getModel()->numSpecies());
      Eigen::MatrixXd ios_covariances(config.getModel()->numSpecies(), config.getModel()->numSpecies());
      Eigen::VectorXd thirdOrder(config.getModel()->numSpecies());

      model->fullState(scanResult[pid], concentrations, lna_covariances, emre_corrections,
                       ios_covariances, thirdOrder, iosemre_corrections);

      parameterScan(pid,0) = GiNaC::ex_to<GiNaC::numeric>(parameterSets[pid][config.getParameter().getIdentifier()]).to_double();

      int col=1;

      // output LNA
      for (size_t i=0; i<config.getNumSpecies(); i++)
        parameterScan(pid,col++) = concentrations(index_table[i]);
      for (size_t i=0; i<config.getNumSpecies(); i++)
        for (size_t j=i; j<config.getNumSpecies(); j++)
            parameterScan(pid,col++) = lna_covariances(index_table[i], index_table[j]);

      // output IOS
      for (size_t i=0; i<config.getNumSpecies(); i++)
        parameterScan(pid,col++) = concentrations(index_table[i])+emre_corrections(index_table[i]);
      for (size_t i=0; i<config.getNumSpecies(); i++)
        for (size_t j=i; j<config.getNumSpecies(); j++)
            parameterScan(pid,col++) = lna_covariances(index_table[i], index_table[j])+ios_covariances(index_table[i], index_table[j]);

  }

  // Done...
  this->setState(Task::DONE);

  {
   iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
   message << "Finished parameter scan.";
   iNA::Utils::Logger::get().log(message);
  }

}


QString
ParamScanTask::getLabel()
{
  return "Steady State Analysis (SSE)";
}


const iNA::Ast::Unit &
ParamScanTask::getSpeciesUnit() const
{
  return config.getModel()->getSpecies(0)->getUnit();
}


const QString &
ParamScanTask::getSpeciesName(int i)
{
  return this->species_name[i];
}

Table & ParamScanTask::getParameterScan() { return parameterScan; }
