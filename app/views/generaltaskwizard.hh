#ifndef __INA_APP_MODELS_GENERALTASKWIZARD_HH__
#define __INA_APP_MODELS_GENERALTASKWIZARD_HH__

#include <QWizard>
#include <QWizardPage>
#include <QComboBox>
#include <QListWidget>
#include <QCheckBox>
#include <QSpinBox>

#include "../models/generaltaskconfig.hh"


/**
 * Base class of all task wizards.
 */
class GeneralTaskWizard : public QWizard
{
  Q_OBJECT

public:
  /** Default constructor. */
  explicit GeneralTaskWizard(QWidget *parent = 0);

  /** Returns a weak reference to the task configuration instance. */
  virtual GeneralTaskConfig &getConfig() = 0;

  /** Simpler interface to the config object with type-cast. */
  template <class ConfigType> ConfigType & getConfigCast() {
    return dynamic_cast<ConfigType &>(getConfig());
  }
};



/**
 * Implements a simple model selection wizard page.
 */
class ModelSelectionWizardPage: public QWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  ModelSelectionWizardPage(GeneralTaskWizard *parent);

  /** Simply resets the combo-box. */
  virtual void initializePage();
  /** Sets the document of the selected model to the config object held by the associated wizard. */
  virtual bool validatePage();

private:
  /** The list of models. */
  QComboBox *_modelSelection;
};



/**
 * Simple wizard page to select some species out of a model that has been selected in a
 * previous wizard page.
 */
class SpeciesSelectionWizardPage : public QWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SpeciesSelectionWizardPage(GeneralTaskWizard *parent);

  /** Polulates list of available species. */
  virtual void initializePage();
  /** Assembles list of selected species. */
  virtual bool validatePage();

private:
  /** Holds the list of available species. */
  QListWidget *speciesList;
};



/**
 * Simple wizard to select and configure the execution engine.
 */
class EngineWizardPage : public QWizardPage
{
  Q_OBJECT

protected:
  /** If true, the wizard will show engines for parallel execution. */
  bool _show_parallel_engine;

public:
  /** Constructor. */
  explicit EngineWizardPage(GeneralTaskWizard *parent, bool show_parallel_engine=true);

  /** Sets the selected engine and configuration. */
  virtual bool validatePage();


private:
  /** Holds the list of available engines. */
  QComboBox *engineList;

  /** Enables optimization. */
  QCheckBox *codeOpt;

  /** Holds the number of threads. */
  QSpinBox  *numThreads;

private slots:
  /** Callback if the engine was changed. */
  void engineSelected(int index);
};



/**
 * Simple wizard page to configure an ODE integrator.
 */
class IntegratorWizardPage : public QWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit IntegratorWizardPage(QWidget *parent=0);

  /** Checks if all parameters are within their limits. */
  virtual bool validatePage();

private slots:
  /** Enables/Disables some parameters, depending on the selected integrator type. */
  void onIntegratorSelected(int index);

private:
  QComboBox *integrator;
  QLineEdit *intermediateSteps;
  QLineEdit *ep_abs;
  QLineEdit *ep_rel;
};



#endif // GENERALTASKWIZARD_HH
