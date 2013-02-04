#include "paramscantask.hh"
#include "eval/eval.hh"

/* ******************************************************************************************* *
 * Implementation of ParamScanTask::Config, the task configuration.
 * ******************************************************************************************* */
ParamScanTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(),
    _model(0), num_threads(0), max_iterations(0), max_time_step(0), epsilon(0),
    parameter(), start_value(0), end_value(1),
    steps(1)
{
  // Pass...
}

ParamScanTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other),
    _model(other._model), selected_method(other.selected_method), num_threads(other.num_threads), max_iterations(other.max_iterations), max_time_step(other.max_time_step),
    epsilon(other.epsilon),
    parameter(other.parameter), start_value(other.start_value), end_value(other.end_value),     steps(other.steps),
    re_model(other.re_model), lna_model(other.lna_model), ios_model(other.ios_model)
{
  // Pass...
}

void
ParamScanTask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);

  // Construct IOS model from SBML model associated with the selected document
  this->_model = new iNA::Models::IOSmodel(document->getModel());

  ModelSelectionTaskConfig::setModelDocument(document);

  // Construct analysis model depending on the selected method:
  switch (selected_method) {
  case RE_ANALYSIS:
    this->re_model = new iNA::Models::REmodel(document->getModel());
    this->lna_model = 0;
    this->ios_model = 0;
    break;

  case LNA_ANALYSIS:
    this->re_model = 0;
    this->lna_model = new iNA::Models::LNAmodel(document->getModel());
    this->ios_model = 0;
    break;

  case IOS_ANALYSIS:
    this->re_model = 0;
    this->lna_model = 0;
    this->ios_model = new iNA::Models::IOSmodel(document->getModel());
    break;

  case UNDEFINED_ANALYSIS:
    this->re_model = 0;
    this->lna_model = 0;
    this->ios_model = 0;
    break;
  }


}

iNA::Models::REmodel *ParamScanTask::Config::getModel() const
{
  switch (selected_method) {
  case RE_ANALYSIS:
    return this->re_model;

  case LNA_ANALYSIS:
    return this->lna_model;

  case IOS_ANALYSIS:
    return this->ios_model;

  default:
    break;
  }

  return 0;
}


void
ParamScanTask::Config::setMethod(SSEMethod method)
{
  selected_method = method;
}


ParamScanTask::Config::SSEMethod
ParamScanTask::Config::getMethod() const
{
  return selected_method;
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
    : Task(parent), config(config), _Ns(config.getModel()->numSpecies()), parameterScan(1,1)
{

    std::vector<QString> species_name(_Ns);
    // Make space

    switch(config.getMethod())
    {
        case Config::RE_ANALYSIS:
            parameterScan.resize(1+_Ns, config.getSteps()+1); break;
        case Config::LNA_ANALYSIS:
            parameterScan.resize(1+_Ns+_Ns*(_Ns+1)/2, config.getSteps()+1); break;
        case Config::IOS_ANALYSIS:
            parameterScan.resize(1+2*_Ns+_Ns*(_Ns+1), config.getSteps()+1); break;
        default:
            break;
    }

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

    if(config.getMethod()==Config::RE_ANALYSIS) return;

    for (size_t i=0; i<_Ns; i++)
      for (size_t j=i; j<_Ns; j++, column++)
        this->parameterScan.setColumnName(
              column,QString("LNA cov(%1,%2)").arg(species_name[i]).arg(species_name[j]));


    if(config.getMethod()==Config::LNA_ANALYSIS) return;

        for (size_t i=0; i<_Ns; i++, column++)
          this->parameterScan.setColumnName(
                column, QString("EMRE(%1)").arg(species_name[i]));

        for (size_t i=0; i<_Ns; i++)
          for (size_t j=i; j<_Ns; j++, column++)
            this->parameterScan.setColumnName(
                  column,QString("IOS cov(%1,%2)").arg(species_name[i]).arg(species_name[j]));

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

  // Allocate result matrix (of unified state vectors)
  std::vector<Eigen::VectorXd> scanResult(config.getSteps()+1);

  // Do parameter scan
  if(config.getMethod()==Config::RE_ANALYSIS)
  {

    if(config.getEngine()==EngineTaskConfig::JIT_ENGINE)
    {
      iNA::Models::ParameterScan<iNA::Models::REmodel, iNA::Eval::jit::Engine<Eigen::VectorXd>, iNA::Eval::jit::Engine<Eigen::VectorXd,Eigen::MatrixXd> >
          pscan(dynamic_cast<iNA::Models::REmodel &>(*config.getModel()),
                config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep());
      pscan.parameterScan(parameterSets,scanResult,config.getOptLevel());
    }
    else
    {
      iNA::Models::ParameterScan<iNA::Models::REmodel>
          pscan(dynamic_cast<iNA::Models::REmodel &>(*config.getModel()),
                config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep());
      pscan.parameterScan(parameterSets,scanResult,config.getOptLevel());
    }

  }
  if(config.getMethod()==Config::LNA_ANALYSIS)
  {
    if(config.getEngine()==EngineTaskConfig::JIT_ENGINE)
    {
      iNA::Models::ParameterScan<iNA::Models::LNAmodel, iNA::Eval::jit::Engine<Eigen::VectorXd>, iNA::Eval::jit::Engine<Eigen::VectorXd,Eigen::MatrixXd> >
          pscan(dynamic_cast<iNA::Models::LNAmodel &>(*config.getModel()),
                config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep());
      pscan.parameterScan(parameterSets,scanResult,config.getOptLevel());
    }
    else
    {
      iNA::Models::ParameterScan<iNA::Models::LNAmodel>
          pscan(dynamic_cast<iNA::Models::LNAmodel &>(*config.getModel()),
                config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep());
      pscan.parameterScan(parameterSets,scanResult,config.getOptLevel());
    }
  }
  if(config.getMethod()==Config::IOS_ANALYSIS)
  {
    if(config.getEngine()==EngineTaskConfig::JIT_ENGINE)
    {
      iNA::Models::ParameterScan<iNA::Models::IOSmodel, iNA::Eval::jit::Engine<Eigen::VectorXd>, iNA::Eval::jit::Engine<Eigen::VectorXd,Eigen::MatrixXd> >
          pscan(dynamic_cast<iNA::Models::IOSmodel &>(*config.getModel()),
                config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep());
      pscan.parameterScan(parameterSets,scanResult,config.getOptLevel());
      std::cerr<<"yes";
    }
    else
    {
      iNA::Models::ParameterScan<iNA::Models::IOSmodel>
          pscan(dynamic_cast<iNA::Models::IOSmodel &>(*config.getModel()),
                config.getMaxIterations(), config.getEpsilon(), config.getMaxTimeStep());
      pscan.parameterScan(parameterSets,scanResult,config.getOptLevel());
    }
  }


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

      // Get information on initial conditions
      iNA::Trafo::excludeType ptab = config.getModel()->makeExclusionTable(parameterSets[pid]);
      iNA::Models::InitialConditions ICs(*config.getModel(),ptab);

      switch(config.getMethod())
      {
         case Config::RE_ANALYSIS:
             dynamic_cast<iNA::Models::REmodel *>(config.getModel())->fullState(ICs,scanResult[pid], concentrations);
             break;
         case Config::LNA_ANALYSIS:
             dynamic_cast<iNA::Models::LNAmodel *>(config.getModel())->fullState(ICs, scanResult[pid], concentrations, lna_covariances);
             break;
         case Config::IOS_ANALYSIS:
            dynamic_cast<iNA::Models::IOSmodel *>(config.getModel())->fullState(ICs, scanResult[pid], concentrations, lna_covariances, emre_corrections,
                           ios_covariances, thirdOrder, iosemre_corrections);
            break;
      default:
          break;
      }
      parameterScan(pid,0) = GiNaC::ex_to<GiNaC::numeric>(parameterSets[pid][config.getParameter().getIdentifier()]).to_double();

      int col=1;

      // output REs
      for (size_t i=0; i<_Ns; i++)
          parameterScan(pid,col++) = concentrations(i);

      if(config.getMethod()==Config::RE_ANALYSIS) continue;

      // output LNA
      for (size_t i=0; i<_Ns; i++)
        for (size_t j=i; j<_Ns; j++)
            parameterScan(pid,col++) = lna_covariances(i,j);

      if(config.getMethod()==Config::LNA_ANALYSIS) continue;

      // output IOS
      for (size_t i=0; i<_Ns; i++)
        parameterScan(pid,col++) = concentrations(i)+emre_corrections(i);
      for (size_t i=0; i<_Ns; i++)
        for (size_t j=i; j<_Ns; j++)
            parameterScan(pid,col++) = lna_covariances(i,j)+ios_covariances(i,j);
  }

  // Done...
  this->setState(Task::DONE);

  {
   iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::INFO);
   message << "Finished parameter scan.";
   iNA::Utils::Logger::get().log(message);
  }

}


const ParamScanTask::Config &
ParamScanTask::getConfig() const {
  return config;
}


QString
ParamScanTask::getLabel()
{
  switch(config.getMethod())
  {
  case Config::RE_ANALYSIS:
      return "Parameter Scan (RE)";
  case Config::LNA_ANALYSIS:
      return "Parameter Scan (LNA)";
  case Config::IOS_ANALYSIS:
      return "Parameter Scan (IOS)";
  default:
      return "";
  }
}


iNA::Ast::Unit
ParamScanTask::getSpeciesUnit() const
{
  return config.getModel()->getSpeciesUnit();
}


Table & ParamScanTask::getParameterScan() { return parameterScan; }
