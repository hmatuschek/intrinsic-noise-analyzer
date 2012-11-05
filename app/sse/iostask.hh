#ifndef __INA_APP_SSE_IOSTASK_HH__
#define __INA_APP_SSE_IOSTASK_HH__

#include <QObject>
#include <QStringList>

#include "../models/task.hh"
#include "../models/timeseries.hh"
#include "ssetaskconfig.hh"

#include <models/sseinterpreter.hh>
#include <ode/integrationrange.hh>
#include <ode/stepper.hh>


/** Represents a task for the IOS (inverse omega square) time-course analysis.
 * @ingroup gui */
class IOSTask : public Task
{
  Q_OBJECT

protected:
  /** Holds the configuration for the task. */
  SSETaskConfig config;

  /** Holds the number of species in the selected model. */
  size_t _Ns;

  /** Holds an instance of the bytecode interpreter for the LNA model, this object also implements
   * the @c System interface for the integrators. */
  iNA::Models::SSEInterpreterInterface *interpreter;

  /** Holds a weak reference to the stepper being used. */
  iNA::ODE::Stepper *stepper;

  /** This table will hold the results of the integration as a time-series.
   * Lets assume there are N species, then this table will have
   * 1 + 3*N + (N*(N+1)) columns. The first column holds the integration time,
   * the next N columns hold the mean for the N selected species (RE) for each time-step. The next
   * N*(N+1)/2 columns hold the vectorized upper-triangular part of the LNA covariance matrix.
   * The triangular covariance matrix is vectorized row-by-row. The next N columns hold the EMRE
   * corrections + RE means and the next N*(N+1)/2 column holds the LNA+IOS covariance. Finally,
   * the last N columns holds the EMRE-IOS mean corrections. */
  Table timeseries;

  /** Holds the list of the names of the selected species. */
  QVector<QString> species_names;

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

  /** If true, at least one of the variance estimators is negative. */
  bool has_negative_variance;


public:
  /** Constructs a Task. */
  explicit IOSTask(const SSETaskConfig &config, QObject *parent = 0);

  /** Destructor. */
  virtual ~IOSTask();

  /** Returns the display name of the i-th selected species. */
  const QString &getSpeciesName(size_t i) const;

  /** Retunrs the vecrtor of display names of the selected species. */
  const QVector<QString> &getSpeciesNames() const;

  /** Returns the time-series table. */
  Table *getTimeSeries();

  /** Returns the (common) unit of the species. */
  iNA::Ast::Unit getSpeciesUnit() const;

  /** Returns the (common) unit of time. */
  iNA::Ast::Unit getTimeUnit() const;

  /** Returns the task label. */
  virtual QString getLabel();

  /** Returns true, if there was at least one variance element negative. */
  bool hasNegativeVariance() const;

  /** Returns the SSE configuration for of the task. */
  inline const SSETaskConfig &getConfig() const { return config; }


protected:
  /** Performs the analysis, will be run in a separate thread. */
  virtual void process();

private:
  /** Creates the interpreter instance. */
  void instantiateInterpreter();
};

#endif // __INA_APP_SSE_IOSTASK_HH__
