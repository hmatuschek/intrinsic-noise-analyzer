#ifndef __INA_APP_SSA_SSATASK_HH__
#define __INA_APP_SSA_SSATASK_HH__

#include "../models/task.hh"
#include "../models/timeseries.hh"
#include "../models/generaltaskconfig.hh"

#include <models/models.hh>
#include <QStringList>


class SSATaskConfig:
    public GeneralTaskConfig,
    public ModelSelectionTaskConfig,
    public EngineTaskConfig
{
public:
  /** Enumerates the available methods. */
  typedef enum {
    DIRECT_SSA,
    OPTIMIZED_SSA,
  } SSAMethod;


protected:
  SSAMethod   method;
  size_t      ensemble_size;
  double      final_time;
  size_t      steps;
  size_t      num_threads;
  iNA::Ast::Model *model;
  iNA::Models::StochasticSimulator *simulator;


public:
  /** Default constructor. */
  SSATaskConfig();

  /** Copy constructor. */
  SSATaskConfig(const SSATaskConfig &other);

  iNA::Models::StochasticSimulator *getSimulator() const;
  void setSimulator(iNA::Models::StochasticSimulator *sim);

  virtual void setModelDocument(DocumentItem *document);
  virtual iNA::Ast::Model *getModel() const;

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
  iNA::Models::StochasticSimulator *simulator;
  size_t _Ns;
  double final_time;
  size_t time_steps;
  Table time_series;

public:
  SSATask(const SSATaskConfig &config, QObject *parent=0);

  Table &getTimeSeries();

  iNA::Models::StochasticSimulator *getModel();

  virtual QString getLabel();


protected:
  virtual void process();
};


#endif // __INA_APP_SSA_SSATASK_HH__
