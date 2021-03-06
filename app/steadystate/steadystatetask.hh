#ifndef __INA_APP_STEADYSTATE_STEADYSTATETASK_HH__
#define __INA_APP_STEADYSTATE_STEADYSTATETASK_HH__

#include "../models/task.hh"
#include "../models/generaltaskconfig.hh"
#include "../models/timeseries.hh"

#include <models/IOSmodel.hh>
#include <models/steadystateanalysis.hh>


/** A task to perform the steady state analysis. */
class SteadyStateTask : public Task
{
  Q_OBJECT

public:
  /** This class assembles all parameters needed to configure a SteadyStateTask. */
  class Config :
      public GeneralTaskConfig,
      public ModelSelectionTaskConfig
  {
  protected:
    iNA::Models::IOSmodel *model;

    int max_iterations;
    double max_time_step;
    double epsilon;

    bool auto_frequencies;
    double min_frequency;
    double max_frequency;
    size_t num_frequency;

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
  };


protected:
  /** Holds the task configuration. */
  Config config;
  /** Holds the number of species in the selected model. */
  size_t _Ns;
  /** Holds an instance of the analysis. */
  iNA::Models::SteadyStateAnalysis<iNA::Models::IOSmodel> steady_state;
  Eigen::VectorXd re_concentrations;
  Eigen::VectorXd emre_corrections;
  Eigen::VectorXd ios_corrections;
  Eigen::MatrixXd lna_covariances;
  Eigen::MatrixXd ios_covariances;

  //Eigen::VectorXd frequencies;
  Table spectrum;

  QVector<size_t> index_table;


public:
  explicit SteadyStateTask(const Config &config, QObject *parent=0);

  Eigen::VectorXd &getConcentrations();

  Eigen::VectorXd &getEMRECorrections();

  Eigen::VectorXd &getIOSCorrections();

  Eigen::MatrixXd &getLNACovariances();

  Eigen::MatrixXd &getIOSCovariances();

  Table &getSpectrum();

  QString getSpeciesId(int i);

  QString getSpeciesName(int i);

  iNA::Ast::Unit getSpeciesUnit() const;

  virtual QString getLabel();

  inline const Config &getConfig() const { return config; }

protected:
  virtual void process();
};

#endif // __INA_APP_STEADYSTATE_STEADYSTATETASK_HH__
