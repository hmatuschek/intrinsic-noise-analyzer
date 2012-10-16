#ifndef SSAWIZARD_HH
#define SSAWIZARD_HH

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include "../doctree/documentitem.hh"
#include "ssatask.hh"

#include "models/models.hh"
#include "../views/generaltaskwizard.hh"


class SSAWizard : public GeneralTaskWizard
{
  Q_OBJECT

public:
  typedef enum
  {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    ENGINE_SELECTION_PAGE,
    CONFIG_PAGE,
    SUMMARY_PAGE
  } PageId;


public:
  explicit SSAWizard(QWidget *parent = 0);

  virtual GeneralTaskConfig &getConfig();

private:
  SSATaskConfig config;
};



class SSAModelSelectionPage : public ModelSelectionWizardPage
{
  Q_OBJECT

public:
  explicit SSAModelSelectionPage(GeneralTaskWizard *parent);

  virtual bool validatePage();
};


class SSASpeciesSelectionPage : public SpeciesSelectionWizardPage
{
  Q_OBJECT

public:
  explicit SSASpeciesSelectionPage(GeneralTaskWizard *parent);
};


class SSAEngineSelectionPage : public EngineWizardPage
{
  Q_OBJECT

public:
  explicit SSAEngineSelectionPage(GeneralTaskWizard *parent);
};


class SSAConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSAConfigPage(SSAWizard *wizard);

  virtual bool validatePage();

private:
  QComboBox *method;
};


class SSASummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSASummaryPage(SSAWizard *wizard);

  virtual void initializePage();

  virtual bool validatePage();


private:
  QLabel *document;
  QLabel *selected_species;
  QLabel *ensemble_size;
  QLabel *final_time;
  QLabel *num_samples;
  QLabel *mem_usage;
  QLabel *method;
  QLabel *thread_count;
};



#endif // SSAWIZARD_HH
