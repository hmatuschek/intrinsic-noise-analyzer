#ifndef __INA_APP_MODELS_GENERALTASKCONFIG_HH__
#define __INA_APP_MODELS_GENERALTASKCONFIG_HH__

#include <QObject>
#include "../doctree/documentitem.hh"
#include "ode/integrationrange.hh"


/**
 * Abstract @c Task config class.
 *
 * This class implements nothing, but defines the virtual base-type for all configuration
 * mixins, that implments secialized configurations for specific tasks. For each configuration
 * mixin, there is a corresponding wizzard-page class, that performs the actual UI part of the
 * configuration.
 */
class GeneralTaskConfig : public QObject
{
  Q_OBJECT

public:
  /** Constructor. */
  GeneralTaskConfig(QObject *parent=0);
  /** Destructor. */
  virtual ~GeneralTaskConfig();
};


/**
 * Virtual configuration interface for task-configs to deal with model selections.
 */
class ModelSelectionTaskConfig
{
protected:
  /**
   * Will hold a weak reference to the selected document, will be null, if no document is selected.
   */
  DocumentItem *document;

public:
  /** Default constructor. */
  ModelSelectionTaskConfig();
  /** Copy constructor. */
  ModelSelectionTaskConfig(const ModelSelectionTaskConfig &other);
  /** Destructor. */
  virtual ~ModelSelectionTaskConfig();
  /** (Re-) Sets the document item of the model being selected for analysis. */
  virtual void setModelDocument(DocumentItem *document);
  /** Returns the currently selected document item of the model to be analyzed. */
  virtual DocumentItem *getModelDocument();
};


/**
 * Virtual configuration interface for task-configs that selects species from a @c Ast::Model.
 */
class SpeciesSelectionTaskConfig
{
protected:
  /** Holds the list of identifiers of the selected species. */
  QStringList selected_species;

public:
  /** Default constructor. */
  SpeciesSelectionTaskConfig();
  /** Copy constructor. */
  SpeciesSelectionTaskConfig(const SpeciesSelectionTaskConfig &other);
  /** Destructor. */
  virtual ~SpeciesSelectionTaskConfig();

  /** Needs to be implemented by the actual task config to return the previously selected model. */
  virtual Fluc::Ast::Model *getModel() const = 0 ;
  /** Returns the list of selected species. */
  virtual const QStringList &getSelectedSpecies() const;
  /** Just returns the number of selected species. */
  size_t getNumSpecies() const;
  /** (Re-) Sets the list of selected species. */
  virtual void setSelectedSpecies(const QStringList &list);
};



/** Virtual configuration interface for the execution engine to use. */
class EngineTaskConfig
{
public:
  /** Defines the execution engine to use. */
  typedef enum {
    GINAC_ENGINE,  ///< Just kidding.
    BCI_ENGINE,    ///< The default byte code interpreter.
    BCIMP_ENGINE,  ///< BCI + OpenMP.
    JIT_ENGINE     ///< JIT comiler engine via LLVM.
  } EngineKind;


protected:
  /** Holds the selected engine. */
  EngineKind _engine;

  /** Holds the optimization level of code. */
  size_t _optLevel;

  /** Holds the number of threads to use for evaluation. */
  size_t _numEvalThreads;


public:
  /** Default constructor, sets the engine to @c BCI_ENGINE. */
  EngineTaskConfig();

  /** Copy constructor. */
  EngineTaskConfig(const EngineTaskConfig &other);

  /** Destructor. */
  virtual ~EngineTaskConfig();

  /** Returns the selected engine. */
  virtual EngineKind getEngine() const;
  /** Resets the selected engine. */
  virtual void setEngine(EngineKind kind);

  /** Returns the code optimization level. */
  virtual size_t getOptLevel() const;
  /** Sets the code optimization level. */
  virtual void setOptLevel(size_t level);

  /** Returns the number of threads to use for expression evaluation. */
  virtual size_t getNumEvalThreads() const;
  virtual void setNumEvalThreads(size_t num);
};



/**
 * Virtual configuration interface for ODE integrator tasks.
 */
class ODEIntTaskConfig
{
public:
  /** This type defines all integrator class identifier. */
  typedef enum {
    RungeKutta4,           ///< The non-adaptive RK4 integrator.
    RungeKuttaFehlberg45,  ///< The adaptive RKF45 integrator.
    LSODA,                 ///< Lsoda integrator.
    DormandPrince5,        ///< Precise Dopri integrator.
    Rosenbrock4            ///< Implicit integrator for stiff systems.
  } Integrator;


protected:
  /** Holds the selected integrator type. */
  Integrator integrator;
  /** Holds the selected integration range. */
  Fluc::ODE::IntegrationRange integration_range;
  /** Optional, holds the itermediate steps (for RK4). */
  size_t intermediate_steps;
  /** Holds the absolut error for adaptive integrators. */
  double epsilon_abs;
  /** Holds the relative error for adaptive integrators. */
  double epsilon_rel;

public:
  /** Default constructor. */
  ODEIntTaskConfig();
  /** Copy constructor. */
  ODEIntTaskConfig(const ODEIntTaskConfig &other);
  /** Destructor. */
  virtual ~ODEIntTaskConfig();

  /** Returns the selected integrator identifier. */
  virtual Integrator getIntegrator() const;
  /** (Re-) Sets the default integrator. */
  virtual void setIntegrator(Integrator integrator);
  /** Returns the selected integration range. */
  virtual const Fluc::ODE::IntegrationRange &getIntegrationRange() const;
  /** (Re-) Sets the integration range. */
  virtual void setIntegrationRange(const Fluc::ODE::IntegrationRange &range);
  /** (Re-) Sets the integration range. */
  virtual void setIntegrationRange(double t_end, size_t steps);
  /** Returns the number of intermediate steps. */
  virtual size_t getIntermediateSteps() const;
  /** (Re-) Sets the number of intermediate steps. */
  virtual void setIntermediateSteps(size_t steps);
  /** Returns the absolute error. */
  virtual double getEpsilonAbs() const;
  /** (Re-) Sets the absolute error. */
  virtual void setEpsilonAbs(double epsilon);
  /** Retunrs the relative error. */
  virtual double getEpsilonRel() const;
  /** (Re-) Sets the relative error. */
  virtual void setEpsilonRel(double epsilon);
  /** (Re-) Sets absolute and relative error. */
  virtual void setEpsilon(double abs, double rel);
};



#endif // __INA_APP_MODELS_GENERALTASKCONFIG_HH__
