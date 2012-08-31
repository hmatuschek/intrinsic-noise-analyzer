#include "ssscantask.hh"

/* ******************************************************************************************* *
 * Implementation of SSScanTask::Config, the task configuration.
 * ******************************************************************************************* */
SSScanTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), SpeciesSelectionTaskConfig(),
    model(0), max_iterations(0), max_time_step(0), epsilon(0),
    parameter(), start_value(0), end_value(1),
    steps(1)
{
  // Pass...
}

SSScanTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), SpeciesSelectionTaskConfig(other),
    model(other.model), max_iterations(other.max_iterations), max_time_step(other.max_time_step),
    epsilon(other.epsilon),
    parameter(other.parameter), start_value(other.start_value),
    end_value(other.end_value), steps(other.steps)
{
  // Pass...
}

void
SSScanTask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);
  // Construct IOS model from SBML model associated with the selected document
  this->model = new iNA::Models::IOSmodel(document->getModel());
}

iNA::Ast::Model *
SSScanTask::Config::getModel() const
{
  return this->model;
}


size_t
SSScanTask::Config::getMaxIterations() const
{
  return this->max_iterations;
}

void
SSScanTask::Config::setMaxIterations(size_t num)
{
  this->max_iterations = num;
}

double
SSScanTask::Config::getMaxTimeStep() const
{
  return max_time_step;
}

void
SSScanTask::Config::setMaxTimeStep(double t_max)
{
  max_time_step = t_max;
}

double
SSScanTask::Config::getEpsilon() const
{
  return this->epsilon;
}

void
SSScanTask::Config::setEpsilon(double eps)
{
  this->epsilon = eps;
}



/* ******************************************************************************************* *
 * Implementation of SSScanTask::Config, the task configuration.
 * ******************************************************************************************* */
SSScanTask::SSScanTask(const Config &config, QObject *parent)
  : Task(parent), config(config),
    steady_state(dynamic_cast<iNA::Models::IOSmodel &>(*config.getModel()),
      config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep()),
    species(config.getNumSpecies()), species_name(config.getNumSpecies()),
    parameterScan(1+2*config.getNumSpecies()+(config.getNumSpecies()*(config.getNumSpecies()+1))/2, config.getSteps()),
    index_table(config.getNumSpecies())
{

    size_t column = 0;

    this->parameterScan.setColumnName(column++, config.getParameter().getName().c_str());
    for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
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
    {
        this->parameterScan.setColumnName(
              column,QString("var(%1)").arg(species_name[i]));
    }

    for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
    {
      this->parameterScan.setColumnName(
            column, QString("EMRE(%1)").arg(species_name[i]));
    }

    for (int i=0; i<(int)config.getNumSpecies(); i++)
    {
        this->parameterScan.setColumnName(
              column,QString("var(%1)").arg(species_name[i]));
    }

}



void
SSScanTask::process()
{

  this->setState(Task::INITIALIZED);
  this->setProgress(0);

  // Construct parameter sets
  std::vector<GiNaC::exmap> parameterSets(config.getSteps());
  {
    size_t j=0;
    for(double val=config.getStartValue(); val<=config.getEndValue(); val+=config.getStepSize())
    {
      parameterSets[j++].insert(std::pair<GiNaC::ex,GiNaC::ex>(config.getParameter().getSymbol(),val));
    }
  }

  // Take model
  iNA::Models::IOSmodel *lna_model
      = dynamic_cast<iNA::Models::IOSmodel *>(config.getModel());

  // Allocate result matrix (of unified state vectors)
  std::vector<Eigen::VectorXd> scanResult(lna_model->getDimension());

  // Do parameter scan
  this->steady_state.parameterScan(parameterSets,scanResult);

  // Check if task shall terminate:
  if (Task::TERMINATING == this->getState())
  {
    this->setState(Task::ERROR, tr("Task was terminated by user."));
    return;
  }


  // Some temporary vectors for the result of the analysis
  Eigen::VectorXd concentrations(config.getModel()->numSpecies());
  Eigen::VectorXd emre_corrections(config.getModel()->numSpecies());
  Eigen::VectorXd iosemre_corrections(config.getModel()->numSpecies());
  Eigen::MatrixXd lna_covariances(config.getModel()->numSpecies(), config.getModel()->numSpecies());
  Eigen::MatrixXd ios_covariances(config.getModel()->numSpecies(), config.getModel()->numSpecies());
  Eigen::VectorXd thirdOrder(config.getModel()->numSpecies());

  // Fill table
  for(size_t j=0; j<scanResult.size(); j++)
  {
      lna_model->fullState(scanResult[j], concentrations, lna_covariances, emre_corrections,
                       ios_covariances, thirdOrder, iosemre_corrections);

      parameterScan(j,0) = GiNaC::ex_to<GiNaC::numeric>(parameterSets[j][config.getParameter().getSymbol()]).to_double();

      int col=1;
      for (int i=0; i<this->species.size(); i++)
        parameterScan(j,col++) = concentrations(index_table[i]);
      for (int i=0; i<this->species.size(); i++)
        parameterScan(j,col++) = lna_covariances(index_table[i], index_table[i]);
      for (int i=0; i<this->species.size(); i++)
        parameterScan(j,col++) = concentrations(index_table[i])+emre_corrections(index_table[i]);
      for (int i=0; i<this->species.size(); i++)
        parameterScan(j,col++) = lna_covariances(index_table[i], index_table[i])+ios_covariances(index_table[i], index_table[i]);

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
SSScanTask::getLabel()
{
  return "Steady State Analysis (SSE)";
}


const iNA::Ast::Unit &
SSScanTask::getSpeciesUnit() const
{
  return config.getModel()->getSpecies(0)->getUnit();
}

const QString &
SSScanTask::getSpeciesId(int i)
{
  return this->species[i];
}

const QString &
SSScanTask::getSpeciesName(int i)
{
  return this->species_name[i];
}

Table & SSScanTask::getParameterScan() { return parameterScan; }
