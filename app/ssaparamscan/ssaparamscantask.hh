#ifndef __INA_APP_SSAPARAMSCAN_TASK_HH__
#define __INA_APP_SSAPARAMSCAN_TASK_HH__

#include "../models/task.hh"
#include "../models/generaltaskconfig.hh"
#include "../models/timeseries.hh"

#include <models/stochasticsimulator.hh>
#include <models/steadystateanalysis.hh>


/** A task to perform a paramter scan. */
class SSAParamScanTask : public Task
{
  Q_OBJECT

public:
  /**
   * This class assembles all parameters needed to configure a parameter scan task.
   */
  class Config :
      public GeneralTaskConfig,
      public ModelSelectionTaskConfig
  {

  public:


  protected:
    iNA::Ast::Model *_model;

    iNA::Ast::Parameter * parameter;
    double start_value;
    double end_value;
    size_t steps;

    double t_transient, t_max, timestep;

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


public:
  explicit SSAParamScanTask(const Config &config, QObject *parent=0);

  Table &getParameterScan();

  iNA::Ast::Unit getSpeciesUnit() const;

  virtual QString getLabel();

  const SSAParamScanTask::Config & getConfig() const;

protected:
  virtual void process();
};

#endif
