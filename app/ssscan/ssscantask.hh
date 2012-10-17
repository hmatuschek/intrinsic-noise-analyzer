#ifndef __INA_APP_SSSCAN_TASK_HH__
#define __INA_APP_SSSCAN_TASK_HH__

#include "../task.hh"
#include "models/IOSmodel.hh"
#include "models/steadystateanalysis.hh"
#include "../timeseries.hh"
#include "../models/generaltaskconfig.hh"


class ParamScanTask : public Task
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
  protected:
    iNA::Models::IOSmodel *_model;

    size_t num_threads;

    int max_iterations;
    double max_time_step;
    double epsilon;

    iNA::Ast::Parameter * parameter;
    double start_value;
    double end_value;
    size_t steps;

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

    /** Returns the max number of iterations.*/
    size_t getMaxIterations() const;
    /** Resets the max number of iterations. */
    void setMaxIterations(size_t num);

    /** Returns the max time step. */
    double getMaxTimeStep() const;
    /** Sets the max time step. */
    void setMaxTimeStep(double t_max);

    /** Returns epsilon. */
    double getEpsilon() const;
    void setEpsilon(double eps);

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
  /** Holds an instance of the analysis. */
  iNA::Models::SteadyStateAnalysis<iNA::Models::IOSmodel> steady_state;
  /** Will hold the results. */
  Table parameterScan;


public:
  explicit ParamScanTask(const Config &config, QObject *parent=0);

  Table &getParameterScan();

  iNA::Ast::Unit getSpeciesUnit() const;

  virtual QString getLabel();

  const ParamScanTask::Config & getConfig() const;

protected:
  virtual void process();
};

#endif
