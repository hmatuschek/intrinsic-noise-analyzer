#ifndef __INA_STOCHASTICSIMULATOR_HH__
#define __INA_STOCHASTICSIMULATOR_HH__

#include "mersennetwister.hh"

#include "ast/ast.hh"
#include <ginac/ginac.h>

#include "basemodel.hh"
#include "particlenumbersmixin.hh"
#include "reasonablemodelmixin.hh"
#include "openmp.hh"


namespace iNA {
namespace Models {

template<class T>
struct less_second
: std::binary_function<T,T,bool>
{
   inline bool operator()(const T& lhs, const T& rhs)
   {
      return lhs.second < rhs.second;
   }
};

/**
 * Implements a 1D histogram.
 *
 * @ingroup ssa
 */

template <typename T, typename U = T>
class Histogram
{

public:

    typedef std::map<T,U> histType;


    histType histogram;

    Histogram()
    {
        // pass...
    };

    void insert(const Eigen::Matrix<T, Eigen::Dynamic, 1> &observation)

    {
        for(int sid=0; sid < observation.rows(); sid++)
        {
            T val = observation(sid,0);
            typename histType::iterator it = histogram.find(val);
            if( it == histogram.end() )
                histogram.insert( std::make_pair<T,U>(val,1.) );
            else
                it->second+=1.;
        }
    }

    U getNorm()

    {
        U norm = 0.;
        for(typename histType::iterator it=histogram.begin(); it!=histogram.end(); it++)
            norm += it->second;
        return norm;
    }

    histType getHistogram()

    {
        return histogram;
    }

    histType getNormalized()

    {

        U norm = getNorm();

        histType nHist(histogram);
        for(typename histType::iterator it=nHist.begin(); it != nHist.end(); it++)
            it->second/=norm;

        return nHist;

    }

    histType getDensity()

    {

        histType density(getNormalized());

        for(typename histType::iterator it=density.begin(); it!=density.end()-1; it++)
            it->second/=((*it).first-(*it+1).first);

        // remove last element
        density.erase(density.end());

        return density;

    }




};


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
  *  Evaluates the histogram of a species from current state
  **/
  void getHistogram(size_t specId, Histogram<double> &hist);

  /**
  * Returns the ensemble size
  **/
  size_t size();
 };


}
}

#endif
