#include "iostask.hh"
#include "ode/ode.hh"
#include "utils/logger.hh"

using namespace Fluc;


IOSTask::Config::Config()
  : GeneralTaskConfig(), ModelSelectionTaskConfig(), SpeciesSelectionTaskConfig(),
    ODEIntTaskConfig(), model(0)
{
  // pass...
}

IOSTask::Config::Config(const Config &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other), SpeciesSelectionTaskConfig(other),
    ODEIntTaskConfig(other), model(other.model)
{
  // Pass...
}


void
IOSTask::Config::setModelDocument(DocumentItem *document)
{
  ModelSelectionTaskConfig::setModelDocument(document);
  // Construct LNA model from SBML model associated with the selected document
  this->model = new Fluc::Models::LinearNoiseApproximation(document->getSBMLModel());
}


Fluc::Ast::Model *
IOSTask::Config::getModel() const
{
  return this->model;
}



/* ******************************************************************************************** *
 * Implementation of IOSTask
 * ******************************************************************************************** */
IOSTask::IOSTask(const Config &config, QObject *parent) :
  Task(parent), config(config),
  interpreter(*(config.model), config.getOptLevel(), config.getNumThreads(), false),
  timeseries(
    1 + 4*config.getNumSpecies() + config.getNumSpecies()*(config.getNumSpecies()+1),
    1+config.getIntegrationRange().getSteps()/(1+config.getIntermediateSteps())),
  re_index_table(config.getNumSpecies()),
  lna_index_table(config.getNumSpecies(), config.getNumSpecies()),
  emre_index_table(config.getNumSpecies()),
  ios_index_table(config.getNumSpecies(), config.getNumSpecies()),
  ios_emre_index_table(config.getNumSpecies()),
  skewness_index_table(config.getNumSpecies())
{
  // Assemble index tables and assign column names to time-series table:
  size_t column = 0;
  QVector<QString> species_names(config.getNumSpecies());

  // Time column:
  this->timeseries.setColumnName(column, "t"); column++;

  // RE means and initialize vector of species names
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++)
  {
    QString species_id = config.getSelectedSpecies().value(i);
    Fluc::Ast::Species *species = config.model->getSpecies(species_id.toStdString());

    if (species->hasName())
      species_names[i] = QString("%1").arg(species->getName().c_str());
    else
      species_names[i] = species_id;

    this->timeseries.setColumnName(column, QString("RE %1").arg(species_names[i]));
    this->re_index_table(i) = column;
  }

  // LNA Covariance
  for (int i=0; i<(int)config.getNumSpecies(); i++) {
    for (int j=i; j<(int)config.getNumSpecies(); j++, column++) {
      this->timeseries.setColumnName(
            column, QString("LNA cov(%1,%2)").arg(species_names[i]).arg(species_names[j]));
      this->lna_index_table(i,j) = column;
      this->lna_index_table(j,i) = column;
    }
  }

  // EMRE means
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++) {
    this->timeseries.setColumnName(
          column, QString("EMRE %1").arg(species_names[i]));
    this->emre_index_table(i) = column;
  }

  // IOS Covariance
  for (int i=0; i<(int)config.getNumSpecies(); i++) {
    for (int j=i; j<(int)config.getNumSpecies(); j++, column++) {
      this->timeseries.setColumnName(
            column, QString("IOS cov(%1,%2)").arg(species_names[i]).arg(species_names[j]));
      this->ios_index_table(i,j) = column;
      this->ios_index_table(j,i) = column;
    }
  }

  // IOS EMRE Mean corrections
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++) {
    this->timeseries.setColumnName(
          column, QString("IOS-EMRE %1").arg(species_names[i]));
    this->ios_emre_index_table(i) = column;
  }

  // IOS Skewness
  for (int i=0; i<(int)config.getNumSpecies(); i++, column++) {
    this->timeseries.setColumnName(
          column, QString("Skew %1").arg(species_names[i]));
    this->skewness_index_table(i) = column;
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



IOSTask::~IOSTask()
{
  // Free integrator:
  if (0 != this->stepper)
    delete stepper;
}


void
IOSTask::process()
{
  {
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "Start IOS analysis.";
    Utils::Logger::get().log(message);
  }

  // Holds the current system state (reduced state)
  Eigen::VectorXd x(config.model->getDimension());
  // Holds the update to the next state (reduced state)
  Eigen::VectorXd dx(config.model->getDimension());

  // Holds the concentrations for each species (full state)
  Eigen::VectorXd concentrations(config.model->numSpecies());

  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd lna(config.model->numSpecies(), config.model->numSpecies());

  // Holds EMRE correction for state:
  Eigen::VectorXd emre(config.model->numSpecies());

  // Holds the covariance matrix of species concentrations
  Eigen::MatrixXd ios(config.model->numSpecies(), config.model->numSpecies());

  // Holds the 3rd moment for each species.
  Eigen::VectorXd thirdMoment = Eigen::VectorXd::Zero(config.model->numSpecies());

  // Holds the iosemre for each species.
  Eigen::VectorXd iosemre(config.model->numSpecies());

  // Holds a row of the output-table:
  Eigen::VectorXd output_vector(timeseries.getNumColumns());

  // Maps the i-th selected species to an index in the concentrations vector:
  size_t N_sel_species = this->config.getNumSpecies();
  std::vector<size_t> species_index(N_sel_species);
  for (size_t i=0; i<N_sel_species; i++)
  {
    species_index[i] = this->config.model->getSpeciesIdx(
          this->config.getSelectedSpecies().value(i).toStdString());
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
  for (size_t i=0; i<N_sel_species; i++)
  {
    size_t index_i = species_index[i];
    output_vector(re_index_table(i)) = concentrations(index_i);
    output_vector(emre_index_table(i)) = concentrations(index_i);
    output_vector(ios_emre_index_table(i)) = concentrations(index_i);
    for (size_t j=i; j<N_sel_species; j++){
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
    if (Task::TERMINATING == this->getState())
    {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    this->setProgress(double(s)/N_steps);

    // Determine update:
    //this->stepper->step(x, t, dx);
    // Update state:
    //x+=dx;

    // Update state:
    this->stepper->step(x, t);
    // Update time
    t += dt;

    // Skip immediate steps
    if(0 != N_intermediate && 0 != s%(1+N_intermediate))
      continue;

    // Get full state:
    config.model->fullState(x, concentrations, lna, emre, ios, thirdMoment, iosemre);

    // store state and time:
    output_vector(0) = t;
    for (size_t i=0; i<N_sel_species; i++)
    {
      size_t index_i = species_index[i];
      output_vector(re_index_table(i)) = concentrations(index_i);
      output_vector(emre_index_table(i)) = emre(index_i) + concentrations(index_i);
      output_vector(ios_emre_index_table(i)) = emre(index_i) + concentrations(index_i) + iosemre(index_i);
      output_vector(skewness_index_table(i)) = thirdMoment(index_i);
      for (size_t j=i; j<N_sel_species; j++){
        size_t index_j = species_index[j];
        output_vector(lna_index_table(i,j)) = lna(index_i, index_j);
        output_vector(ios_index_table(i,j)) = lna(index_i, index_j) + ios(index_i, index_j);
      }
    }
    this->timeseries.append(output_vector);
  }

  // Finally send last progress event:
  this->setProgress(1.0);
  this->setState(Task::DONE);

  {
    Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
    message << "IOS analysis successful.";
    Utils::Logger::get().log(message);
  }
}


QString
IOSTask::getLabel()
{
  return "Time Course Analysis (IOS)";
}


Table *
IOSTask::getTimeSeries()
{
  return &(this->timeseries);
}


const QList<QString> &
IOSTask::getSelectedSpecies() const
{
  return config.getSelectedSpecies();
}


const Fluc::Ast::Unit &
IOSTask::getSpeciesUnit() const
{
  return this->config.model->getConcentrationUnit();
}


const Fluc::Ast::Unit &
IOSTask::getTimeUnit() const
{
  return this->config.model->getTimeUnit();
}
