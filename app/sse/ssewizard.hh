#ifndef IOSWIZARD_HH
#define IOSWIZARD_HH

#include <QWizard>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>

#include "models/linearnoiseapproximation.hh"
#include "../doctree/documentitem.hh"
#include "../views/generaltaskwizard.hh"


#include "ssetaskconfig.hh"


/**
 * Simple wizzard to configure the deterministic time-course analysis.
 */
class SSEWizard : public GeneralTaskWizard
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
  SSETaskConfig config;


public:
  /** Constructor. */
  explicit SSEWizard(QWidget *parent = 0);
  /** Implements GeneralTaskWizard interface. */
  virtual GeneralTaskConfig &getConfig();
};



/**
 * Tiny specialization of @c ModelSelectionWizardPage, that checks if the selected model
 * can be analyzed using deterministic REs (almost all models can be analyzed this way).
 */
class SSEModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SSEModelSelectionPage(GeneralTaskWizard *parent);

  /** Simply checks if a REModel can in instantiated for the selected SBML model. */
  virtual bool validatePage();

private:
  QRadioButton *_re_button;
  QRadioButton *_lna_button;
  QRadioButton *_ios_button;
};


/**
 * Specialization of SpeciesSelectionWizardPage.
 */
class SSESpeciesSelectionPage : public SpeciesSelectionWizardPage
{
public:
  /** Constructor. */
  explicit SSESpeciesSelectionPage(GeneralTaskWizard *parent);
};


/**
 * Specialization of ODEIntWizardPage.
 */
class SSEIntegratorPage : public IntegratorWizardPage
{
public:
  /** Constructor. */
  explicit SSEIntegratorPage(GeneralTaskWizard *parent);
};


/**
 * Simple summary page to finalize configuration.
 */
class SSESummaryPage : public QWizardPage
{
public:
  /** Constructor. */
  explicit SSESummaryPage(GeneralTaskWizard *parent);

  virtual void initializePage();

private:
  QLabel *model_name;
  QLabel *selected_species;
  QLabel *integration_range;
  QLabel *memory;
};

#endif // WIZARD_HH
