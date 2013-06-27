#ifndef RETASK_HH
#define RETASK_HH

#include <QObject>
#include <QStringList>

#include "../models/task.hh"
#include "../models/generaltaskconfig.hh"
#include "../models/timeseries.hh"

#include "ssetaskconfig.hh"

#include <models/sseinterpreter.hh>
#include <models/REmodel.hh>
#include <ode/integrationrange.hh>
#include <ode/stepper.hh>


/**
 * Represents a task for the deterministic time-course analysis.
 *
 * @ingroup gui
 */
class RETask : public Task
{
  Q_OBJECT


protected:
  /** Holds the configuration for the task. */
  SSETaskConfig config;

  /** Reference to sseModel. */
  iNA::Models::REmodel * _sseModel;

  /** Holds the number of species in the selected model. */
  size_t _Ns;

  /** Holds an instance of the bytecode interpreter for the LNA model, this object also implements
   * the @c System interface for the integrators. */
  iNA::Models::SSEInterpreterInterface *interpreter;

  /** Holds a weak reference to the stepper being used. */
  iNA::ODE::Stepper *stepper;

  /** This table will hold the results of the integration as a time-series.
   *
   * Lets assume there are N , then this table will have
   * 1 + N + (N*(N+1))/2 + N columns. The first column holds the integration time,
   * the next N columns hold the mean for the N selected species for each time-step. The next
   * N*(N+1)/2 columns hold the vectorized upper-triangular part of the LNA covariance matrix.
   * The triangular covariance matrix is vectorized row-by-row. The last N columns hold the EMRE
   * corrections + LNA means. */
  Table timeseries;


public:
  /** Constructs a Task. */
  explicit RETask(SSETaskConfig &config, QObject *parent = 0);

  /** Destructor. */
  virtual ~RETask();

  /** Returns the time-series table. */
  Table *getTimeSeries();

  /** Returns the (common) unit of the species. */
  iNA::Ast::Unit getSpeciesUnit() const;

  /** Returns the (common) unit of time. */
  const iNA::Ast::Unit &getTimeUnit() const;

  /** Returns the task label. */
  virtual QString getLabel();

  /** Returns the task config. */
  const SSETaskConfig &getConfig() const;

protected:
  /** Performs the analysis, will be run in a separate thread. */
  virtual void process();
};

#endif // RETASK_HH
