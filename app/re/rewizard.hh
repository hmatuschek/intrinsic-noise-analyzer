#ifndef REWIZARD_HH
#define REWIZARD_HH

#include <QWizard>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>

#include "models/linearnoiseapproximation.hh"
#include "../doctree/documentitem.hh"
#include "../views/generaltaskwizard.hh"

#include "retask.hh"


/**
 * Simple wizzard to configure the deterministic time-course analysis.
 */
class REWizard : public GeneralTaskWizard
{
  Q_OBJECT

public:
  /**
   * Wizzard page identifier.
   */
  typedef enum {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    INTEGRATOR_CONFIG_PAGE,
    SUMMARY_PAGE
  } PageId;


protected:
  /**
   * The task configuration.
   *
   * This instance will be populated by the wizzard.
   */
  RETask::Config config;


public:
  /** Constructor. */
  explicit REWizard(QWidget *parent = 0);
  /** Implements GeneralTaskWizard interface. */
  virtual GeneralTaskConfig &getConfig();
};



/**
 * Tiny specialization of @c ModelSelectionWizardPage, that checks if the selected model
 * can be analyzed using deterministic REs (almost all models can be analyzed this way).
 */
class REModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit REModelSelectionPage(GeneralTaskWizard *parent);

  /** Simply checks if a REModel can in instantiated for the selected SBML model. */
  virtual bool validatePage();
};


/**
 * Specialization of SpeciesSelectionWizardPage.
 */
class RESpeciesSelectionPage : public SpeciesSelectionWizardPage
{
public:
  /** Constructor. */
  explicit RESpeciesSelectionPage(GeneralTaskWizard *parent);
};


/**
 * Specialization of ODEIntWizardPage.
 */
class REIntegratorPage : public IntegratorWizardPage
{
public:
  /** Constructor. */
  explicit REIntegratorPage(GeneralTaskWizard *parent);
};


/**
 * Simple summary page to finalize configuration.
 */
class RESummaryPage : public QWizardPage
{
public:
  /** Constructor. */
  explicit RESummaryPage(GeneralTaskWizard *parent);

  virtual void initializePage();

private:
  QLabel *model_name;
  QLabel *selected_species;
  QLabel *integration_range;
  QLabel *memory;
};

#endif // WIZARD_HH
