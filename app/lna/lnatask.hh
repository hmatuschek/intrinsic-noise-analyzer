#ifndef LNATASK_HH
#define LNATASK_HH

#include <QObject>
#include <QStringList>

#include "../task.hh"
#include "models/lnainterpreter.hh"
#include "models/linearnoiseapproximation.hh"
#include "ode/integrationrange.hh"
#include "ode/stepper.hh"
#include "../timeseries.hh"


/**
 * Represents a Linear Noise Approximation and EMRE correction task.
 *
 * @ingroup gui
 */
class LNATask : public Task
{
  Q_OBJECT

public:
  /**
   * Holds the complete configuration for the LNA task.
   */
  class Config {
  public:
    typedef enum {
      RungeKutta4,
      RungeKuttaFehlberg45,
      DormandPrince5,
      Rosenbrock4
    } Integrator;

  public:
    Config();

    Config(const Config &other);

  public:
    Fluc::Models::LinearNoiseApproximation *model;
    QStringList selected_species;
    Integrator integrator;
    Fluc::ODE::IntegrationRange integration_range;
    size_t immediate_steps;
    size_t numThreads;
    size_t optLevel;
    double epsilon_abs;
    double epsilon_rel;
  };


protected:
  /**
   * Holds the configuration for the task.
   */
  Config config;

  /**
   * Holds an instance of the bytecode interpreter for the LNA model, this object also implements
   * the System interface for the integrators.
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
   * 1 + N + (N*(N+1))/2 + N columns. The first column holds the integration time,
   * the next N columns hold the mean for the N selected species for each time-step. The next
   * N*(N+1)/2 columns hold the vectorized upper-triangular part of the LNA covariance matrix.
   * The triangular covariance matrix is vectorized row-by-row. The last N columns hold the EMRE
   * corrections + LNA means.
   */
  Table timeseries;


public:
  /**
   * Constructs a Task.
   */
  explicit LNATask(const Config &config, QObject *parent = 0);

  virtual ~LNATask();

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

#endif // LNATASK_HH
