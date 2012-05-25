#ifndef IOSTASK_HH
#define IOSTASK_HH

#include <QObject>
#include <QStringList>

#include "../task.hh"
#include "../models/generaltaskconfig.hh"
#include "models/lnainterpreter.hh"
#include "models/linearnoiseapproximation.hh"
#include "ode/integrationrange.hh"
#include "ode/stepper.hh"
#include "../timeseries.hh"


/**
 * Represents a task for the deterministic time-course analysis.
 *
 * @ingroup gui
 */
class IOSTask : public Task
{
  Q_OBJECT

public:
  /**
   * Holds the complete configuration for the LNA task.
   */
  class Config :
      public GeneralTaskConfig,
      public ModelSelectionTaskConfig,
      public SpeciesSelectionTaskConfig,
      public ODEIntTaskConfig
  {
  public:
    /**
     * Default constructor.
     */
    Config();

    /**
     * Copy constructor.
     */
    Config(const Config &other);

    /** Overrides the default implenentation of @c ModelSelectionTaskConfig and checks if
     * the selected model can be used to instantiate a LNAModel. The LNA model instance is then
     * stored in model. */
    virtual void setModelDocument(DocumentItem *document);

    /** Implements the @c SpeciesSelectionTaskConfig interface, and returns the LNA model instance. */
    virtual Fluc::Ast::Model *getModel() const;


  public:
    /** The model to be analyzed. */
    Fluc::Models::LinearNoiseApproximation *model;
  };


protected:
  /**
   * Holds the configuration for the task.
   */
  Config config;

  /**
   * Holds an instance of the bytecode interpreter for the LNA model, this object also implements
   * the @c System interface for the integrators.
   */
  Fluc::Models::LNAinterpreter interpreter;

  /**
   * Holds a weak reference to the stepper being used.
   */
  Fluc::ODE::Stepper *stepper;

  /**
   * This table will hold the results of the integration as a time-series.
   *
   * Lets assume there are N species selected for the analysis, then this table will have
   * 1 + 3*N + (N*(N+1)) columns. The first column holds the integration time,
   * the next N columns hold the mean for the N selected species (RE) for each time-step. The next
   * N*(N+1)/2 columns hold the vectorized upper-triangular part of the LNA covariance matrix.
   * The triangular covariance matrix is vectorized row-by-row. The next N columns hold the EMRE
   * corrections + RE means and the next N*(N+1)/2 column holds the LNA+IOS covariance. Finally,
   * the last N columns holds the EMRE-IOS mean corrections.
   */
  Table timeseries;

  /** Index table for RE means. Maps the i-th selected species to the corresponding
   * column in the timeseries table. */
  Eigen::VectorXi re_index_table;

  /** Index table for the LNA covariance matrix. Maps the i,j-th entry to the corresponding
   * column in the timeseries table. */
  Eigen::MatrixXi lna_index_table;

  /** Index table for the RE+EMRE means. Maps the i-th selected species to the corresponding
   * column in the timeseries table. */
  Eigen::VectorXi emre_index_table;

  /** Index table for the IOS covariance matrix. Maps the i,j-th entry to the corresponding
   * column in the timeseries table. */
  Eigen::MatrixXi ios_index_table;

  /** Index table for the IOS-EMRE mean corrections. Maps the i-th selected species to the
   *  corresponding index in the timeseries table. */
  Eigen::VectorXi ios_emre_index_table;

  /** Index table for the IOS skewness values. Maps the i-th selected species to the corresponding
    * index in the timeseries table. */
  Eigen::VectorXi skewness_index_table;


public:
  /**
   * Constructs a Task.
   */
  explicit IOSTask(const Config &config, QObject *parent = 0);

  /**
   * Destructor.
   */
  virtual ~IOSTask();

  /**
   * Returns the list of selected species.
   */
  const QList<QString> &getSelectedSpecies() const;

  /**
   * Returns the time-series table.
   */
  Table *getTimeSeries();

  /**
   * Returns the (common) unit of the species.
   */
  const Fluc::Ast::Unit &getSpeciesUnit() const;

  /**
   * Returns the (common) unit of time.
   */
  const Fluc::Ast::Unit &getTimeUnit() const;

  /**
   * Returns the task label.
   */
  virtual QString getLabel();


protected:
  /**
   * Performs the analysis, will be run in a separate thread.
   */
  virtual void process();
};

#endif // RETASK_HH
