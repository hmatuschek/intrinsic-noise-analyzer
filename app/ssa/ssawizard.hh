#ifndef SSAWIZARD_HH
#define SSAWIZARD_HH

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include "../doctree/documentitem.hh"
#include "ssatask.hh"

#include "models/models.hh"



class SSAWizard : public QWizard
{
  Q_OBJECT

public:
  typedef enum
  {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    CONFIG_PAGE,
    SUMMARY_PAGE
  } PageId;


public:
  explicit SSAWizard(QWidget *parent = 0);

  void setDocument(DocumentItem *document);
  DocumentItem *getDocument();

  void setSelectedSpecies(const QList<QString> &species);
  const QList<QString> &getSelectedSpecies();

  void setEnsembleSize(size_t size);
  size_t getEnsembleSize();

  void setFinalTime(double time);
  double getFinalTime();

  void setNumSamples(size_t size);
  size_t getNumSamples();

  void setMethod(const QString &method);
  const QString &getMethod();

  void setOptimizeByteCode(bool optimize);
  bool getOptimizeByteCode();

  void setThreadCount(size_t count);
  size_t getThreadCount();

  void setSimulator(Fluc::Models::StochasticSimulator *simulator);
  Fluc::Models::StochasticSimulator *getSimulator();

  SSATaskConfig getConfig();


private:
  DocumentItem *document;
  QList<QString> selected_species;
  size_t ensemble_size;
  double final_time;
  size_t num_samples;
  QString method;
  bool optimize_bytecode;
  size_t thread_count;
  Fluc::Models::StochasticSimulator *simulator;
};



class SSAModelSelectionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSAModelSelectionPage(QWidget *parent=0);

  virtual bool validatePage();


private:
  QComboBox *modelSelection;
};


class SSASpeciesSelectionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSASpeciesSelectionPage(QWidget *parent=0);

  virtual void initializePage();

  virtual bool validatePage();

private:
  QListWidget *speciesList;
};


class SSAConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSAConfigPage(QWidget *parent=0);

  virtual bool validatePage();

private:
  QComboBox *method;
};


class SSASummaryPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit SSASummaryPage(QWidget *parent=0);

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
  QLabel *optimize_bytecode;
  QLabel *thread_count;
};



#endif // SSAWIZARD_HH
