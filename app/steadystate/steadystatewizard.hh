#ifndef __INA_APP_STEADYSTATE_STEADYSTATEWIZARD_HH__
#define __INA_APP_STEADYSTATE_STEADYSTATEWIZARD_HH__

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>


#include "../doctree/documentitem.hh"
#include "steadystatetask.hh"
#include "../views/generaltaskwizard.hh"


class SteadyStateWizard : public GeneralTaskWizard
{
  Q_OBJECT

public:
  /** This enumarates the available wizard pages. */
  typedef enum {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    FREQUENCY_RANGE_PAGE,
    SUMMARY_PAGE
  } PageId;

protected:
  /** The task configuration. This instance will be populated by the wizard. */
  SteadyStateTask::Config config;

public:
  /** Default constructor. */
  explicit SteadyStateWizard(QWidget *parent = 0);
  /** Implements GeneralTaskWizard interface. */
  virtual GeneralTaskConfig &getConfig();
};



class LNASteadyStateModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  explicit LNASteadyStateModelSelectionPage(GeneralTaskWizard *parent);
  virtual bool validatePage();
};


class LNASteadyStateSpectrumConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASteadyStateSpectrumConfigPage(GeneralTaskWizard *parent);

  virtual bool validatePage();

private:
  QLineEdit *f_min;
  QLineEdit *f_max;
  QLineEdit *f_num;
  QLineEdit *n_iter;
  QLineEdit *epsilon;
  QLineEdit *t_max;

private slots:
  void fAutomaticToggled(bool value);
};



class LNASteadyStateSummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASteadyStateSummaryPage(QWidget *parent=0);

  virtual void initializePage();

private:
  QLabel *model_name;
  QLabel *spectrum;
  QLabel *memory;
};


#endif // LNASTEADYSTATEWIZARD_HH