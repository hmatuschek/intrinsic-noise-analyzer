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



class SteadyStateModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  explicit SteadyStateModelSelectionPage(GeneralTaskWizard *parent);
  virtual bool validatePage();
};


class SteadyStateSpectrumConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SteadyStateSpectrumConfigPage(GeneralTaskWizard *parent);

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



class SteadyStateSummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SteadyStateSummaryPage(QWidget *parent=0);

  virtual void initializePage();

private:
  QLabel *model_name;
  QLabel *spectrum;
  QLabel *memory;
};


#endif // LNASTEADYSTATEWIZARD_HH
