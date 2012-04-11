#ifndef LNASTEADYSTATETASK_HH
#define LNASTEADYSTATETASK_HH

#include "../task.hh"
#include "models/linearnoiseapproximation.hh"
#include "models/steadystateanalysis.hh"
#include "../timeseries.hh"


class LNASteadyStateTask : public Task
{
  Q_OBJECT


protected:
  Fluc::Models::LinearNoiseApproximation *model;
  Fluc::Models::SteadyStateAnalysis steady_state;
  int max_iterations;
  double epsilon;

  Eigen::VectorXd concentrations;
  Eigen::VectorXd emre_corrections;
  Eigen::MatrixXd covariances;

  bool auto_frequencies;
  Eigen::VectorXd frequencies;
  Table spectrum;

  QVector<QString> species;
  QVector<QString> species_name;

  QVector<size_t> index_table;


public:
  explicit LNASteadyStateTask(Fluc::Models::LinearNoiseApproximation *model,
                              const QList<QString> &selected_species,
                              bool auto_frequencies,
                              size_t num_freqs, double min_freq, double max_freq,
                              size_t max_iterations, double epsilon, QObject *parent=0);

  Eigen::VectorXd &getConcentrations();

  Eigen::VectorXd &getEMRECorrections();

  Eigen::MatrixXd &getCovariances();

  Table &getSpectrum();

  const QString &getSpeciesId(int i);

  const QString &getSpeciesName(int i);

  const Fluc::Ast::Unit &getSpeciesUnit() const;

  Fluc::Models::LinearNoiseApproximation *getModel();

  virtual QString getLabel();


protected:
  virtual void process();
};

#endif // LNASTEADYSTATETASK_HH
