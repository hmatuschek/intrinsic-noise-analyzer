#ifndef __FLUC_MODELS_STEADYSTATEANALYSIS_HH
#define __FLUC_MODELS_STEADYSTATEANALYSIS_HH

#include "nlesolve/nlesolve.hh"
#include "linearnoiseapproximation.hh"
#include "REmodel.hh"
#include "math.hh"

namespace Fluc {
namespace Models {

/**
 * @ingroup models
 *
 * Performs steady state statistics on a LinearNoiseApproximation model.
 *
 * The NLEsolve::NewtonRaphson solvers is used for the deterministic REs.
 */
class SteadyStateAnalysis
{

protected:
  /**
   * Holds a reference to a LNA instance.
   */
   LinearNoiseApproximation &lnaModel;

   /**
    * Holds an instance of NLESolver::NewtonRaphson solver.
    */
   NLEsolve::NewtonRaphson<LinearNoiseApproximation> solver;

public:

   /**
   * Constructor
   */
  SteadyStateAnalysis(LinearNoiseApproximation &model);

  /**
  * Constructor
  */
  SteadyStateAnalysis(LinearNoiseApproximation &model,const size_t &iter,const double &epsilon);

  /**
  * Set the precision of the root finding algorithm
  */
  void setPrecision(const double &epsilon);

  /**
  * Set the maximum number of iterations used by the root finding algorithm
  */
  void setMaxIterations(const size_t &maxiter);

  /**
   * Solves rate equations for steady state concentrations in @c conc and returns the number
   * of function evalutions.
   */
  int calcConcentrations(Eigen::VectorXd &conc);

  /**
   * Get the LNA steady state spectrum.
   *
   * @param freq Specifies the vector of frequencies to evaluate the spectrum on. If @c automatic
   *        is true, the vector will be ignored and the frequencies are selected automatically. On
   *        exit the vector contains the selected frequencies.
   * @param spectrum On exit, contains the spectrum for each species.
   * @param automatic Specifies if the frequencies of the spectrum are selected automatically.
   */
  void calcSpectrum(Eigen::VectorXd &freq, Eigen::MatrixXd &spectrum, bool automatic=true);

  /**
   * Solves for steady state of the reduced state vector and returns number of function evaluations
   * used.
   *
   * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
   *        coordinates. Contents of x will be overwritten.
   */
  int calcSteadyState(Eigen::VectorXd &x);

  /**
   * Solves for LNA steady state of the reduced state vector and returns number of function evaluations
   * used.
   *
   * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
   *        coordinates. Contents of x will be overwritten.
   */

  int calcLNA(Eigen::VectorXd &x);

  /**
   * Solves for IOS steady state of the reduced state vector and returns number of function evaluations
   * used.
   *
   * @param x: Outputs the steady state concentrations, covariance and EMRE vector, the IOS covariance corrections and the IOS emre corrections in reduced
   *        coordinates. Contents of x will be overwritten.
   */

  int calcIOS(Eigen::VectorXd &x);

  /**
   * Samples the frequencies [0, f_max], where f_max is dertermined by the eigen values of the
   * Jacobian.
   */
  void getFrequencyRange(Eigen::VectorXd &freq);

  /**
   * Get maximum frequency from Jacobian estimation.
   */
  double getMaxFrequency(const double &pr);

  /**
   * Dump all the nice values of the steady state analysis.
   */
  void dump(std::ostream &str);

};


/**
 * Provides a functor of the REs and the transposed Jacobian for HybridNonLinearSolver provided by
 * Eigen.
 *
 * All expressions are evaluated using Ginac.
 *
 * @deprecated We do not use the solver from Eigen, so this class can be removed.
 *
 * @ingroup models
 */
class REevaluator
{
protected:
  /**
   * Holds a reference to an instance of @c LinearNoiseApproximation.
   */
   LinearNoiseApproximation &lnaModel;

public:
   /**
    * Constructor
    */
    REevaluator(LinearNoiseApproximation &model)
        : lnaModel(model)
    {}

    /**
     * Evaluates the REs
     */
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec)
    {
        lnaModel.getREs(x,fvec);
        return 0;
    }

    /**
     * Evaluates the transposed Jacobian
     */
    int df(const Eigen::VectorXd &x, Eigen::MatrixXd &fjac)
    {
        lnaModel.getJacobianMatrix(x,fjac);
        fjac.transposeInPlace();
        return 0;
    }
};

}} // close namespaces

#endif // __FLUC_MODELS_STEADYSTATEANALYSIS_HH
