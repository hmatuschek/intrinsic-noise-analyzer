#ifndef __INA_APP_SSAPARAMSCAN_TASK_HH__
#define __INA_APP_SSAPARAMSCAN_TASK_HH__

#include <vector>
#include "../models/task.hh"
#include "../models/generaltaskconfig.hh"
#include "../models/timeseries.hh"

#include <models/stochasticsimulator.hh>
#include <models/steadystateanalysis.hh>
#include <models/ssaparamscan.hh>



/** A task to perform a paramter scan. */
class SSAParamScanTask : public Task
{
  Q_OBJECT

public:
  /** This class assembles all parameters needed to configure a parameter scan task. */
  class Config :
      public GeneralTaskConfig,
      public ModelSelectionTaskConfig,
      public EngineTaskConfig
  {

  protected:
    /** The model to scan. */
    iNA::Ast::Model *_model;
    /** The parameter we scan. */
    iNA::Ast::Parameter * parameter;
    /** Start value of the parameter. */
    double start_value;
    /** End value of the parameter. */
    double end_value;
    /** Number of steps of the parameter. */
    size_t steps;
    /** Scan parameters i.e. the transient time (time to reach steady state,
     * maximum simulation time and the time step at which samples are taken. */
    double t_transient, t_max, timestep;
    /** Number of threads to use for simulation. */
    size_t num_threads;

  public:
    /** Default constructor. */
    Config();

    /** Copy constructor. */
    Config(const Config &other);

    /** Overrides the default implenentation of @c ModelSelectionTaskConfig and checks if
     * the selected model can be used to instantiate a LNAModel. The LNA model instance is then
     * stored in model. */
    virtual void setModelDocument(DocumentItem *document);

    /** Implements the @c SpeciesSelectionTaskConfig interface, and returns the LNA model instance. */
    virtual iNA::Ast::Model *getModel() const;

    /** Sets the number of threads for OpenMP. */
    void setNumThreads(size_t num);
    size_t getNumThreads() const;

    /** Returns the max time of simulation.*/
    double getMaxTime() const;
    /** Resets the max time of simulation. */
    void setMaxTime(double num);

    /** Returns the timestep. */
    double getTimeStep() const;
    /** Sets the timestep. */
    void setTimeStep(double timestep);

    /** Returns transient time. */
    double getTransientTime() const;
    void setTransientTime(double t_transient);

    inline const iNA::Ast::Parameter &getParameter() const { return *parameter; }
    inline void setParameter(iNA::Ast::Parameter *id) { parameter = id; }
    inline double getStartValue() const { return start_value; }
    inline void setStartValue(double value) { start_value = value; }
    inline double getEndValue() const { return end_value; }
    inline void setEndValue(double value) { end_value = value; }
    size_t getSteps() const { return steps; }
    void setSteps(size_t value) { steps = value; }
    double getInterval() const { return (end_value-start_value)/steps; }

  };


protected:
  /** Holds the task configuration. */
  Config config;
  /** Holds the number of species defined in the selected model. */
  size_t _Ns;
  /** Will hold the results. */
  Table parameterScan;
  /** If true, the iteration will be stopped. */
  bool _stop_iteration;
  /** If true, resets the statistics of the scan. */
  bool _reset_stats;
  /** Holds the current iteration number. */
  size_t _current_iteration;
  /** Holds the simulation time. */
  double _simulation_time;

public:
  explicit SSAParamScanTask(const Config &config, QObject *parent=0);
  virtual ~SSAParamScanTask();

  Table &getParameterScan();

  iNA::Ast::Unit getSpeciesUnit() const;

  virtual QString getLabel();

  const SSAParamScanTask::Config & getConfig() const;

  size_t currentIteration() const;
  double currentTime() const;

  void stopIteration();
  void resetStatistics();

signals:
  /** Will be emitted each time a simulation step was performed. */
  void stepPerformed();


protected:
  /** On the first call, calls @c initializeScan and then @c performStep, for every further call
   * simply calls @c performStep to continue simulation. */
  virtual void process();
  /** Initializes the parameter scan. */
  void initializeScan();
  /** Performs a simulation step, the simulation can be continued by restarting the thread. */
  void performStep();
};


#endif
