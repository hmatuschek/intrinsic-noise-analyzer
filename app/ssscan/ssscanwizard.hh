#ifndef __INA_APP_SSSCAN_WIZARD_HH__
#define __INA_APP_SSSCAN_WIZARD_HH__

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>

#include "ssscantask.hh"
#include "../doctree/documentitem.hh"
#include "../views/generaltaskwizard.hh"


class SSScanWizard : public GeneralTaskWizard
{
  Q_OBJECT

public:
  /** This enumarates the available wizard pages. */
  typedef enum {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    SCAN_CONFIG_PAGE,
    SUMMARY_PAGE
  } PageId;

protected:
  /** The task configuration. This instance will be populated by the wizard. */
  SSScanTask::Config config;

public:
  /** Default constructor. */
  explicit SSScanWizard(QWidget *parent = 0);
  /** Implements GeneralTaskWizard interface. */
  virtual GeneralTaskConfig &getConfig();
};



class SSScanModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  explicit SSScanModelSelectionPage(GeneralTaskWizard *parent);
  virtual bool validatePage();
};



class SSScanSpeciesSelectionPage : public SpeciesSelectionWizardPage
{
  Q_OBJECT

public:
  explicit SSScanSpeciesSelectionPage(GeneralTaskWizard *parent);
};



class SSScanSpectrumConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSScanSpectrumConfigPage(GeneralTaskWizard *parent);

  virtual void initializePage();
  virtual bool validatePage();

private:
  QLineEdit *n_iter;
  QLineEdit *epsilon;
  QLineEdit *t_max;

  QComboBox *p_select;
  QLineEdit *p_min;
  QLineEdit *p_max;
  QLineEdit *p_num;
};



class SSScanSummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSScanSummaryPage(QWidget *parent=0);

  virtual void initializePage();

private:
  QLabel *model_name;
  QLabel *species;
  QLabel *spectrum;
  QLabel *memory;
};


#endif // LNASTEADYSTATEWIZARD_HH
