#include "lnasteadystatetask.hh"



LNASteadyStateTask::LNASteadyStateTask(Fluc::Models::LinearNoiseApproximation *model,
                                       const QList<QString> &selected_species,
                                       bool auto_frequencies,
                                       size_t num_freq, double min_freq, double max_freq,
                                       size_t max_iterations, double epsilon, QObject *parent)
  : Task(parent), model(model), steady_state(*model, max_iterations, epsilon),
    concentrations(selected_species.size()), emre_corrections(selected_species.size()),
    covariances(selected_species.size(), selected_species.size()),
    auto_frequencies(auto_frequencies),
    frequencies(num_freq), spectrum(selected_species.size()+1, num_freq),
    species(selected_species.size()), species_name(selected_species.size()),
    index_table(selected_species.size())
{
  this->spectrum.setColumnName(0, "freq");

  // Assign species identifier:
  for (int i=0; i<this->species.size(); i++)
  {
    this->species[i] = selected_species.at(i);
    std::cerr << "Index of species: " << this->species[i].toStdString()
              << " = " << this->model->getSpeciesIdx(this->species[i].toStdString()) << std::endl;
    this->index_table[i] = this->model->getSpeciesIdx(this->species[i].toStdString());

    if (model->getSpecies(this->species[i].toStdString())->hasName())
    {
      this->species_name[i] = model->getSpecies(this->species[i].toStdString())->getName().c_str();
    }
    else
    {
      this->species_name[i] = this->species[i];
    }

    this->spectrum.setColumnName(i+1, this->species_name[i]);
  }

  // Create frequency vector:
  double df = (max_freq-min_freq)/num_freq;
  for (size_t i=0; i<num_freq; i++)
  {
    this->frequencies(i) = min_freq+i*df;
  }
}



void
LNASteadyStateTask::process()
{
  this->setState(Task::INITIALIZED);

  // Allocate reduced state vector (independent species)
  Eigen::VectorXd reduced_state(this->model->getDimension());

  // signal running:
  this->setState(Task::RUNNING);

  std::cerr << "Calc steady state..." << std::endl;
  // Calc steadystate:
  this->steady_state.calcSteadyState(reduced_state);
  std::cerr << "   ... done." << std::endl;

  // Check if task shall terminate:
  if (Task::TERMINATING == this->getState())
  {
    this->setState(Task::ERROR, tr("Task was terminated by user."));
    return;
  }

  // Get full state and covariance and EMRE corrections for steady state;
  Eigen::VectorXd concentrations(this->model->numSpecies());
  Eigen::VectorXd emre_corrections(this->model->numSpecies());
  Eigen::MatrixXd covariances(this->model->numSpecies(), this->model->numSpecies());
  this->model->fullState(reduced_state, concentrations, covariances, emre_corrections);

//  // Get steadystate spectrum:
//  Eigen::MatrixXd spectrum(this->frequencies.size(), this->model->numSpecies());
//  this->setProgress(0.5);

//  std::cerr << "Calc spectrum ... " << std::endl;
//  this->steady_state.calcSpectrum(this->frequencies, spectrum, auto_frequencies);
//  std::cerr << "   ... done." << std::endl;

  // Copy concentration, EMRE, COVARIANCE
  for (int i=0; i<this->species.size(); i++)
  {
    this->concentrations(i) = concentrations(index_table[i]);
    this->emre_corrections(i) = emre_corrections(index_table[i]);

    for (int j=0; j<this->species.size(); j++)
    {
      this->covariances(i, j) = covariances(index_table[i], index_table[j]);
    }
  }

  // Copy spectrum into table
  for (int i=0; i<this->frequencies.size(); i++)
  {
    this->spectrum.matrix()(i,0) = this->frequencies(i);

    for (int j=0; j<this->species.size(); j++)
    {
      size_t idx = this->index_table[j];
      this->spectrum.matrix()(i,j+1) = spectrum(i, idx);
    }
  }

  // Done...
  this->setProgress(1);
  this->setState(Task::DONE);

  std::cerr << "Finished SteadyStateTask." << std::endl;
}


QString
LNASteadyStateTask::getLabel()
{
  return "Steady State Analysis (SSE)";
}


Fluc::Models::LinearNoiseApproximation *
LNASteadyStateTask::getModel()
{
  return this->model;
}


const Fluc::Ast::Unit &
LNASteadyStateTask::getSpeciesUnit() const
{
  return this->model->getSpecies(0)->getUnit();
}

Eigen::VectorXd &
LNASteadyStateTask::getConcentrations()
{
  return this->concentrations;
}


Eigen::VectorXd &
LNASteadyStateTask::getEMRECorrections()
{
  return this->emre_corrections;
}


Eigen::MatrixXd &
LNASteadyStateTask::getCovariances()
{
  return this->covariances;
}


Table &
LNASteadyStateTask::getSpectrum()
{
  return this->spectrum;
}


const QString &
LNASteadyStateTask::getSpeciesId(int i)
{
  return this->species[i];
}

const QString &
LNASteadyStateTask::getSpeciesName(int i)
{
  return this->species_name[i];
}
