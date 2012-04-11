#ifndef NLAWIZARD_HH
#define NLAWIZARD_HH

#include <QWizard>
#include <QComboBox>
#include <QListWidget>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>

#include "models/linearnoiseapproximation.hh"
#include "../doctree/documentitem.hh"

#include "lnatask.hh"


class LNAWizard : public QWizard
{
  Q_OBJECT

public:
  typedef enum {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    INTEGRATOR_CONFIG_PAGE,
    SUMMARY_PAGE
  } PageId;


protected:
  DocumentItem *document;

  LNATask::Config config;


public:
  explicit LNAWizard(QWidget *parent = 0);

  void setDocument(DocumentItem *doc);
  DocumentItem *getDocument() const;

  LNATask::Config &getTaskConfig();
};



class LNAModelSelectionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNAModelSelectionPage(QWidget *parent=0);

  virtual void initializePage();

  virtual bool validatePage();


private:
  QComboBox *modelSelection;
};



class LNASpeciesSelectionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASpeciesSelectionPage(QWidget *parent=0);

  virtual void initializePage();
  virtual bool validatePage();


private:
  QListWidget *speciesList;
};



class LNAIntegratorConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNAIntegratorConfigPage(QWidget *parent=0);
  virtual bool validatePage();


private slots:
  void onIntegratorSelected(int index);


private:
  QComboBox *integrator;
  QLineEdit *intermediateSteps;
  QCheckBox *optByteCode;
  QSpinBox  *numThreads;
  QLineEdit *ep_abs;
  QLineEdit *ep_rel;
};



class LNASummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASummaryPage(QWidget *parent=0);

  virtual void initializePage();

private:
  QLabel *model_name;
  QLabel *selected_species;
  QLabel *integration_range;
  QLabel *memory;
};

#endif // NLAWIZARD_HH
