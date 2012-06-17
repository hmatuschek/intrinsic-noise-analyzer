#ifndef SSATASK_HH
#define SSATASK_HH

#include "../task.hh"
#include "models/models.hh"
#include "../timeseries.hh"
#include "../models/generaltaskconfig.hh"
#include <QStringList>


class SSATaskConfig:
    public GeneralTaskConfig,
    public ModelSelectionTaskConfig,
    public SpeciesSelectionTaskConfig,
    public EngineTaskConfig
{
public:
  /** Enumerates the available methods. */
  typedef enum {
    DIRECT_SSA,
    OPTIMIZED_SSA
  } SSAMethod;


protected:
  SSAMethod   method;
  size_t      ensemble_size;
  double      final_time;
  size_t      steps;
  size_t      num_threads;
  Fluc::Ast::Model *model;
  Fluc::Models::StochasticSimulator *simulator;


public:
  /** Default constructor. */
  SSATaskConfig();

  /** Copy constructor. */
  SSATaskConfig(const SSATaskConfig &other);

  Fluc::Models::StochasticSimulator *getSimulator() const;
  void setSimulator(Fluc::Models::StochasticSimulator *sim);

  virtual void setModelDocument(DocumentItem *document);
  virtual Fluc::Ast::Model *getModel() const;

  SSAMethod getMethod() const;
  void setMethod(SSAMethod meth);

  size_t getEnsembleSize() const;
  void setEnsembleSize(size_t size);

  double getFinalTime() const;
  void setFinalTime(double ftime);

  size_t getSteps() const;
  void setSteps(size_t steps);

  size_t getNumThreads() const;
  void setNumThreads(size_t num);
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

  Eigen::VectorXi mean_index_table;
  Eigen::MatrixXi cov_index_table;
  Eigen::VectorXi skew_index_table;
  Eigen::VectorXi species_index_table;

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
