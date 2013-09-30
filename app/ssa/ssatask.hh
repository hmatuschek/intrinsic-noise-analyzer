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
  void setSteps(size_t _steps);


protected:
  SSAMethod   _method;
  size_t      _ensemble_size;
  double      _final_time;
  size_t      _steps;
  iNA::Ast::Model *_model;
  iNA::Models::StochasticSimulator *_simulator;
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
