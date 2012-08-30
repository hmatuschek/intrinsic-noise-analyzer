#ifndef __INA_APP_SSSCAN_TASK_HH__
#define __INA_APP_SSSCAN_TASK_HH__

#include "../task.hh"
#include "models/IOSmodel.hh"
#include "models/steadystateanalysis.hh"
#include "../timeseries.hh"
#include "../models/generaltaskconfig.hh"


class SSScanTask : public Task
{
  Q_OBJECT

public:
  /**
   * This class assembles all parameters needed to configure a SteadyStateTask.
   */
  class Config :
      public GeneralTaskConfig,
      public ModelSelectionTaskConfig,
      public SpeciesSelectionTaskConfig
  {
  protected:
    Fluc::Models::IOSmodel *model;

    int max_iterations;
    double max_time_step;
    double epsilon;

    bool auto_frequencies;
    double min_frequency;
    double max_frequency;
    size_t num_frequency;

    std::string parameter;
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
    virtual Fluc::Ast::Model *getModel() const;

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

    inline const std::string &getParameter() const { return paramter; }
    inline void setParameter(const std::string &id) { paramter = id; }
    inline double getStartValue() const { return start_value; }
    inline void setStartValue(double value) { start_value = value; }
    inline double getEndValue() const { return end_value; }
    inline void setEndValue(double value) { end_value = value; }
    size_t getSteps() const { return steps; }
    void setSteps(size_t value) { steps = value; }
    double getStepSize() const { return (end_value-start_value)/steps; }
  };


protected:
  /** Holds the task configuration. */
  Config config;
  /** Holds an instance of the analysis. */
  Fluc::Models::SteadyStateAnalysis<Fluc::Models::IOSmodel> steady_state;
  Eigen::VectorXd concentrations;
  Eigen::VectorXd emre_corrections;
  Eigen::VectorXd ios_corrections;
  Eigen::MatrixXd lna_covariances;
  Eigen::MatrixXd ios_covariances;
  QVector<QString> species;
  QVector<QString> species_name;

  //Eigen::VectorXd frequencies;
  Table spectrum;

  QVector<size_t> index_table;


public:
  explicit LNASteadyStateTask(const Config &config, QObject *parent=0);

  Eigen::VectorXd &getConcentrations();

  Eigen::VectorXd &getEMRECorrections();

  Eigen::VectorXd &getIOSCorrections();

  Eigen::MatrixXd &getLNACovariances();

  Eigen::MatrixXd &getIOSCovariances();

  Table &getSpectrum();

  const QString &getSpeciesId(int i);

  const QString &getSpeciesName(int i);

  const Fluc::Ast::Unit &getSpeciesUnit() const;

  virtual QString getLabel();


protected:
  virtual void process();
};

#endif // LNASTEADYSTATETASK_HH
