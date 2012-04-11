#ifndef SSATASK_HH
#define SSATASK_HH

#include "../task.hh"
#include "models/models.hh"
#include "../timeseries.hh"
#include <QStringList>


class SSATaskConfig
{
public:
  QStringList selected_species;
  double      final_time;
  size_t      steps;
  Fluc::Models::StochasticSimulator *simulator;

public:
  SSATaskConfig(Fluc::Models::StochasticSimulator *simulator,
                const QList<QString> &selected_species,
                double final_time, size_t steps)
    : selected_species(selected_species),
      final_time(final_time), steps(steps),
      simulator(simulator)
  {
    // Pass...
  }
};



class SSATask : public Task
{
  Q_OBJECT

protected:
  Fluc::Models::StochasticSimulator *simulator;

  QVector<QString> species_id;
  QVector<QString> species_name;

  double final_time;
  size_t time_steps;
  Table time_series;

  QVector<size_t> index_table;


public:
  SSATask(const SSATaskConfig &config, QObject *parent=0);

  Table &getTimeSeries();

  size_t numSpecies() const;

  Fluc::Models::StochasticSimulator *getModel();

  virtual QString getLabel();


protected:
  virtual void process();
};


#endif // SSATASK_HH
