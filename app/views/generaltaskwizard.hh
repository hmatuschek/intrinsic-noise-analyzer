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

  /** Retruns a weak reference to the task configuration instance. */
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
  QComboBox *modelSelection;
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
  QCheckBox *optByteCode;
  QSpinBox  *numThreads;
  QLineEdit *ep_abs;
  QLineEdit *ep_rel;
};



#endif // GENERALTASKWIZZARD_HH
