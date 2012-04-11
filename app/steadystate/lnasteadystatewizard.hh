#ifndef LNASTEADYSTATEWIZARD_HH
#define LNASTEADYSTATEWIZARD_HH

#include <QWizard>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>


#include "models/linearnoiseapproximation.hh"
#include "../doctree/documentitem.hh"



class LNASteadyStateWizard : public QWizard
{
  Q_OBJECT

public:
  typedef enum {
    MODEL_SELECTION_PAGE,
    SPECIES_SELECTION_PAGE,
    FREQUENCY_RANGE_PAGE,
    SUMMARY_PAGE
  } PageId;


protected:
  DocumentItem *document;
  Fluc::Models::LinearNoiseApproximation *lna_model;
  QList<QString> selected_species;
  bool frequency_auto_range;
  double frequency_min;
  double frequency_max;
  size_t num_frequencies;
  size_t num_iterations;
  double epsilon;


public:
  explicit LNASteadyStateWizard(QWidget *parent = 0);

  Fluc::Models::LinearNoiseApproximation *getLNAModel();
  void setDocument(DocumentItem *doc);
  DocumentItem *getDocument() const;

  void setSelectedSpecies(const QList<QString> &species);
  const QList<QString> &getSelectedSpecies();

  void setFrequencyAutoRange(bool automatic);
  void setFrequencyRange(double min, double max, size_t num);

  bool getAutomaticFrequencies();
  size_t getNumFrequencies();
  double getMinFrequency();
  double getMaxFrequency();

  void setNumIterations(size_t num);
  size_t getNumIterations();

  void setEpsilon(double eps);
  double getEpsilon();
};



class LNASteadyStateModelSelectionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASteadyStateModelSelectionPage(QWidget *parent=0);

  virtual bool validatePage();


private:
  QComboBox *modelSelection;
};



class LNASteadyStateSpeciesSelectionPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASteadyStateSpeciesSelectionPage(QWidget *parent=0);

  virtual void initializePage();

  virtual bool validatePage();

private:
  QListWidget *speciesList;
};



class LNASteadyStateSpectrumConfigPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit LNASteadyStateSpectrumConfigPage(QWidget *parent=0);

  virtual bool validatePage();

private:
  QLineEdit *f_min;
  QLineEdit *f_max;
  QLineEdit *f_num;
  QLineEdit *n_iter;
  QLineEdit *epsilon;

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
  QLabel *species;
  QLabel *spectrum;
  QLabel *memory;
};


#endif // LNASTEADYSTATEWIZARD_HH
