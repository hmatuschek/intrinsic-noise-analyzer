#ifndef __FLUC_STOCHASTICSIMULATOR_HH__
#define __FLUC_STOCHASTICSIMULATOR_HH__

#include "mersennetwister.hh"

#include "ast/ast.hh"
#include <ginac/ginac.h>

#include "basemodel.hh"
#include "particlenumbersmixin.hh"
#include "reasonablemodelmixin.hh"
#include "openmp.hh"


namespace Fluc {
namespace Models {

/**
 * Base model for all stochastic simulators.
 *
 * @ingroup ssa
 */
class StochasticSimulator :
    public BaseModel,
    public ParticleNumbersMixin,
    public ReasonableModelMixin
{
private:
  /**
   * Number of OpenMP threads to be used.
   */
  size_t num_threads;


protected:
  /**
   * A vector of thread-private RNGs.
   */
  std::vector<MersenneTwister> rand;

  /**
   * needs interpreter to set initial conditons
   **/
  Ast::EvaluateModel interpreter;

  /**
  * index map for bytecode interpreter
  **/
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> stateIndex;

  /**
  * data matrix storing each individual observation
  **/
  Eigen::MatrixXd observationMatrix;

  /**
  * Stores the initial conditions of a simulator.
  **/
  Eigen::VectorXd ics;

  /**
  * Vector storing compartment volumes of reactants
  **/
  Eigen::VectorXd Omega;

  /**
   * Internal storage of ensemble size.
   **/
  int ensembleSize;

  /**
   * evaluate @c propensities given a realization of the vector of population numbers @c populationVec
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
   *
   * @deprecated This constructor will be removed soon.
   **/
  StochasticSimulator(libsbml::Model *model, int ensembleSize, int seed, size_t num_threads=OpenMP::getMaxThreads());

  /**
   * Is initialized with a model, the number of realization @c ensembleSize and a seed for the
   * random number generator
   **/
  StochasticSimulator(const Ast::Model &model, int ensembleSize, int seed, size_t num_threads=OpenMP::getMaxThreads());

  /**
  * Gives number of threads used for OpenMP parallelism
  **/
  const size_t &numThreads();

  /**
  *  stepper
  **/
  virtual void run(double step)=0;

  /**
   * Destructor.
   */
  virtual ~StochasticSimulator();

  /**
  * Returns the observation matrix of concentrations
  **/
  void getState(Eigen::MatrixXd &state);

  /**
  *  Performs the ensemble average of concentration statistics.
  *
  *  @param mean the vector of mean concentrations
  *  @param covariance the lower diagonal covariance matrix
  **/

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
  void getHistogram(size_t specId, std::map<double,double> &hist);

  /**
  * Returns the ensemble size
  **/
  size_t size();
 };

}

}

#endif
