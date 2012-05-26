#include "ssatask.hh"


SSATask::SSATask(const SSATaskConfig &config, QObject *parent)
  : Task(parent), simulator(config.simulator),
    species_id(config.selected_species.size()), species_name(config.selected_species.size()),
    final_time(config.final_time),
    time_series(1+2*species_id.size()+(species_id.size()*(species_id.size()+1))/2, config.steps+1),
    mean_index_table(config.selected_species.size()),
    cov_index_table(config.selected_species.size(), config.selected_species.size()),
    skew_index_table(config.selected_species.size()),
    species_index_table(config.selected_species.size())
{
  // First, column is time:
  this->time_series.setColumnName(0, "t");
  size_t column = 1;
  // Assemble list of species and index_table
  for (int i=0; i<this->species_id.size(); i++, column++)
  {
    // Store species ID
    this->species_id[i] = config.selected_species.at(i);

    // Get name of species or use ID as name:
    if (simulator->getSpecies(this->species_id[i].toStdString())->hasName()) {
      this->species_name[i] = simulator->getSpecies(this->species_id[i].toStdString())->getName().c_str();
    } else {
      this->species_name[i] = this->species_id[i];
    }

    // Get index of species:
    this->mean_index_table[i] = column;
    // Associate i-th selected species with original index:
    this->species_index_table[i] = simulator->getSpeciesIdx(this->species_id[i].toStdString());

    // Assign column name for species:
    this->time_series.setColumnName(column, this->species_name[i]);
  }

  // Assign names to covariances:
  for (int i=0; i<this->species_id.size(); i++)
  {
    for (int j=i; j<this->species_id.size(); j++, column++)
    {
      QString name_a = this->species_name[i];
      QString name_b = this->species_name[j];
      this->time_series.setColumnName(column, QString("cov(%1,%2)").arg(name_a).arg(name_b));
      this->cov_index_table(i,j) = column;
      this->cov_index_table(j,i) = column;
    }
  }

  // Assign names to skewness:
  for (int i=0; i<this->species_id.size(); i++, column++) {
    this->time_series.setColumnName(column, QString("Skewness %1").arg(this->species_name[i]));
    this->skew_index_table[i] = column;
  }
}


void
SSATask::process()
{
  this->setState(Task::INITIALIZED);
  double dt = this->final_time/(this->time_series.getNumRows()-1);

  Eigen::VectorXd mean(this->simulator->numSpecies());
  Eigen::MatrixXd cov(this->simulator->numSpecies(), this->simulator->numSpecies());
  Eigen::VectorXd skewness(this->simulator->numSpecies());

  this->setState(Task::RUNNING);
  this->setProgress(0.0);

  for (size_t i=0; i<this->time_series.getNumRows(); i++)
  {
    // Check if task shall terminate:
    if (Task::TERMINATING == this->getState())
    {
      this->setState(Task::ERROR, tr("Task was terminated by user."));
      return;
    }

    // Set current simulation statistics:
    this->simulator->stats(mean, cov, skewness);

    // Assemble timeseries row:
    Eigen::VectorXd row(this->time_series.getNumColumns());
    row(0) = i*dt;
    for (size_t j=0; j<this->numSpecies(); j++) {
      size_t index_j = this->species_index_table[j];
      row(this->mean_index_table[j]) = mean(index_j);
      row(this->skew_index_table[j]) = skewness(index_j);
      for (size_t k=j; k<this->numSpecies(); k++) {
        size_t index_k = this->species_index_table[k];
        row(this->cov_index_table(j,k)) = cov(index_j, index_k);
      }
    }
    this->time_series.append(row);

    // Perfrom simulation:
    this->simulator->run(dt);

    // Update progress bar:
    this->setProgress(double(i+1)/this->time_series.getNumRows());
  }

  // Done:
  this->setProgress(1.0);
  this->setState(Task::DONE);
}


QString
SSATask::getLabel()
{
  return "Stochastic Simulation Algorithm";
}


Fluc::Models::StochasticSimulator *
SSATask::getModel()
{
  return this->simulator;
}


Table &
SSATask::getTimeSeries()
{
  return this->time_series;
}


size_t
SSATask::numSpecies() const
{
  return this->species_id.size();
}
