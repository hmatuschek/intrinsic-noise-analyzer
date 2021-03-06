#ifndef __INA_STOCHASTICSIMULATOR_HH__
#define __INA_STOCHASTICSIMULATOR_HH__

#include "../mersennetwister.hh"

#include "../ast/ast.hh"
#include "../trafo/assertions.hh"
#include <ginac/ginac.h>

#include "basemodel.hh"
#include "particlenumbersmixin.hh"
#include "histogram.hh"
#include "../openmp.hh"


namespace iNA {
namespace Models {

/**
 * Base model for all stochastic simulators.
 *
 * @ingroup ssa
 */
class StochasticSimulator :
    public BaseModel,
    public ParticleNumbersMixin,
    public Trafo::ReasonableModelMixin
{
  using BaseModel::getConcentrationUnit;

private:
  /** Number of OpenMP threads to be used. */
  size_t num_threads;

protected:
  /** A vector of thread-private RNGs. */
  std::vector<MersenneTwister> rand;

  /** index map for bytecode interpreter */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> stateIndex;

  /** data matrix storing each individual observation */
  Eigen::MatrixXd observationMatrix;

  /** Stores the initial conditions of a simulator. */
  Eigen::VectorXd ics;

  /** Vector containing the values of the compartment volumes for each reactant */
  Eigen::VectorXd Omega;

  /** Internal storage of ensemble size. */
  int ensembleSize;

  /**
   * Evaluate @c propensities given a realization of the vector of population numbers @c populationVec
   **/
  virtual void evaluate(const Eigen::VectorXd &populationVec, Eigen::VectorXd &propensities);

  /**
   * Returns a random number distributed uniform in [0,1).
   */
  virtual double uniform();


public:
  /**
   * Is initialized with a model, the number of realization @c ensembleSize and a seed for the
   * random number generator
   **/
  StochasticSimulator(const Ast::Model &model, int ensembleSize, int seed, size_t num_threads=OpenMP::getMaxThreads(),
                      const std::vector<const GiNaC::symbol *> &parameters=std::vector<const GiNaC::symbol *>());

  /**
  * Gives number of threads used for OpenMP parallelism
  **/
  const size_t &numThreads();

  /**
  *  Stepper
  **/
  virtual void run(double step)=0;

  /**
  *  Resets the state of the ensemble to the initial value
  **/
  void reset();

  /**
   * Destructor.
   */
  virtual ~StochasticSimulator();

  /**
  * Returns the observation matrix of concentrations
  **/
  void getState(Eigen::MatrixXd &state);

  /**
  * Returns the observation matrix of concentrations
  **/
  Eigen::MatrixXd getState() const;

  /**
  *  Performs the ensemble average of concentration statistics.
  *
  *  @param mean the vector of mean concentrations
  *  @param covariance the lower diagonal covariance matrix
  **/


  Eigen::MatrixXd & getObservationMatrix();

  void stats(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance, Eigen::VectorXd &skewness);

  /**
  *  Performs the ensemble average of the flux statistics.
  *
  *  @param mean the vector of mean fluxes
  *  @param covariance of fluxes
  **/
  void fluxStatistics(Eigen::VectorXd &mean, Eigen::MatrixXd &covariance);

  /**
  *  Evaluates the histogram of a species from current state
  **/
  void getHistogram(size_t speciesIdx, std::map<double,double> &hist);

  /**
  *  Evaluates the histogram of a species from current state
  **/
  void getHistogram(size_t specIdx, Histogram<double> &hist);

  /**
  * Returns the ensemble size
  **/
  size_t size();


  Ast::Unit getConcentrationUnit() const;
 };


}
}

#endif
