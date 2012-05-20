#include "ssatask.hh"


SSATask::SSATask(const SSATaskConfig &config, QObject *parent)
  : Task(parent), simulator(config.simulator),
    species_id(config.selected_species.size()), species_name(config.selected_species.size()),
    final_time(config.final_time),
    time_series(1+species_id.size()+(species_id.size()*(species_id.size()+1))/2, config.steps+1),
    index_table(config.selected_species.size())
{
  // First, column is time:
  this->time_series.setColumnName(0, "t");

  // Assemble list of species and index_table
  for (int i=0; i<this->species_id.size(); i++)
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
    this->index_table[i] = simulator->getSpeciesIdx(this->species_id[i].toStdString());

    // Assign column name for species:
    this->time_series.setColumnName(i+1, this->species_name[i]);
  }

  // Assign names to covariances:
  size_t column = 1+this->species_id.size();
  for (int i=0; i<this->species_id.size(); i++)
  {
    for (int j=i; j<this->species_id.size(); j++, column++)
    {
      QString name_a = this->species_name[i];
      QString name_b = this->species_name[j];
      this->time_series.setColumnName(column, QString("cov(%1,%2)").arg(name_a).arg(name_b));
    }
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

    // Store data in table:
    this->time_series.matrix()(i, 0) = i*dt;
    for (size_t j=0; j<this->numSpecies(); j++)
    {
      this->time_series.matrix()(i,j+1) = mean(this->index_table[j]);
    }

    size_t offset = 1+this->numSpecies();
    for (size_t j=0; j<this->numSpecies(); j++)
    {
      for (size_t k=j; k<this->numSpecies(); k++, offset++)
      {
        this->time_series.matrix()(i, offset)
            = cov(this->index_table[j], this->index_table[k]);
      }
    }

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
