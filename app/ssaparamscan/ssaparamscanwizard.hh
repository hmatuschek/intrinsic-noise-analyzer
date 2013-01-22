#ifndef __INA_APP_SSAPARAMSCAN_WIZARD_HH__
#define __INA_APP_SSAPARAMSCAN_WIZARD_HH__

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QRadioButton>

#include "ssaparamscantask.hh"
#include "../doctree/documentitem.hh"
#include "../views/generaltaskwizard.hh"


class SSAParamScanWizard : public GeneralTaskWizard
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
  SSAParamScanTask::Config config;

public:
  /** Default constructor. */
  explicit SSAParamScanWizard(QWidget *parent = 0);
  /** Implements GeneralTaskWizard interface. */
  virtual GeneralTaskConfig &getConfig();
};



class SSAParamScanModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  explicit SSAParamScanModelSelectionPage(GeneralTaskWizard *parent);
  virtual bool validatePage();

private:
  QRadioButton *_re_button;
  QRadioButton *_lna_button;
  QRadioButton *_ios_button;
};



class SSAParameterScanConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSAParameterScanConfigPage(GeneralTaskWizard *parent);

  virtual void initializePage();
  virtual bool validatePage();

private:
  QLineEdit *t_transient;
  QLineEdit *timestep;
  QLineEdit *t_max;

  QComboBox *p_select;
  QLineEdit *p_min;
  QLineEdit *p_max;
  QLineEdit *p_num;

private slots:
  void refreshParamRange(int);

};



class SSAParamScanSummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSAParamScanSummaryPage(QWidget *parent=0);

  virtual void initializePage();

private:

  QLabel *model_name;
  QLabel *param;
  QLabel *spectrum;
  QLabel *memory;
  QLabel *method;
};


#endif // LNASTEADYSTATEWIZARD_HH
